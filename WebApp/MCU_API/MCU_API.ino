/*
 * FoodDPet API REST para ESP32
 * API para gestionar horarios de alimentación y solicitudes
 */

#include "Adafruit_VL53L1X.h"
#include <Wire.h>
#include "time.h"
#include "esp_timer.h"

#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);

#include <ArduinoJson.h>

// Pins for distance sensors
#define SDA_PIN 5
#define SCL_PIN 6
#define IRQ_PIN 2
#define XSHUT_PIN 4

#define TRIGGER_PIN 38
#define ECHO_PIN 37

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

// Configuración para el timer
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -6 * 3600;  // Ajusta según tu país
const int daylightOffset_sec = 0;
volatile bool tick_1s = false;

void IRAM_ATTR timerCallback(void* arg) {
  tick_1s = true;
}

esp_timer_handle_t periodic_timer;

// Configuración WiFi
const char* ssid = "Familia Martinez 2.4";
const char* password = "JimmyAntonio";

// Estructura para Solicitudes
struct FoodRequest {
  String hora;
  String estado;  // "Completed", "Pending", "Cancelled"
};

// Estructura para Horarios
struct Schedule {
  int id;
  String time;
  bool active;
  // Added variables for interrupt
  int hour;
  int minute;
  void (*func)();
  bool triggered;
};

// Datos de prueba - Solicitudes
FoodRequest requests[10];
int requestCount = 4;

// Datos de prueba - Horarios
Schedule schedules[10];
int scheduleCount = 3;

// Estado del dispensador
int fullnessPercentage = 100;

void setup_sensors() {
  // Ultrasonic sensor pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Laser sensor
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);
}

void setup_timer() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando hora...");
  delay(2000);

  // Timer 1 second
  const esp_timer_create_args_t timer_args = {
    .callback = &timerCallback,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "timer_1s"
  };

  esp_timer_create(&timer_args, &periodic_timer);
  esp_timer_start_periodic(periodic_timer, 1'000'000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializar datos de prueba
  initializeTestData();

  // Conectar a WiFi
  Serial.println("\nConectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡WiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // setup both distance sensors
  Serial.println("Configurando sensores");
  setup_sensors();

  // Configurar rutas de la API
  setupRoutes();

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");


  setup_timer();
  Serial.println("Sistema listo.");
}

void loop() {
  if (tick_1s) {
    tick_1s = false;
    check_events();
  }
  server.handleClient();
}

void initializeTestData() {
  // Inicializar solicitudes de prueba
  requests[0] = { "2024-11-15T10:00:00", "Pending" };
  requests[1] = { "2024-11-15T14:30:00", "Cancelled" };
  requests[2] = { "2024-11-15T09:15:00", "Completed" };
  requests[3] = { "2024-11-15T16:45:00", "Pending" };

  // Inicializar horarios de prueba
  schedules[0] = { 1, "12:55", true, 12, 55, dispense_food, false };
  schedules[1] = { 2, "14:00", true, 14, 0, dispense_food, false };
  schedules[2] = { 3, "20:00", false, 20, 0, dispense_food, false };
}

void setupRoutes() {
  // Handler para todas las solicitudes OPTIONS (CORS preflight)
  server.onNotFound([]() {
    if (server.method() == HTTP_OPTIONS) {
      handleCORS();
    } else {
      handleNotFound();
    }
  });

  // Rutas para el Dashboard
  server.on("/api/fullness", HTTP_GET, handleGetFullness);
  server.on("/api/fullness", HTTP_OPTIONS, handleCORS);

  server.on("/api/requests", HTTP_GET, handleGetRequests);
  server.on("/api/requests", HTTP_OPTIONS, handleCORS);

  server.on("/api/requests/complete", HTTP_POST, handleCompleteRequest);
  server.on("/api/requests/complete", HTTP_OPTIONS, handleCORS);

  server.on("/api/requests/cancel", HTTP_POST, handleCancelRequest);
  server.on("/api/requests/cancel", HTTP_OPTIONS, handleCORS);

  server.on("/api/refill", HTTP_POST, handleRefill);
  server.on("/api/refill", HTTP_OPTIONS, handleCORS);

  // Rutas para Horarios
  server.on("/api/schedules", HTTP_GET, handleGetSchedules);
  server.on("/api/schedules", HTTP_POST, handleCreateSchedule);
  server.on("/api/schedules", HTTP_OPTIONS, handleCORS);

  server.on("/api/schedules/update", HTTP_PUT, handleUpdateSchedule);
  server.on("/api/schedules/update", HTTP_OPTIONS, handleCORS);

  server.on("/api/schedules/delete", HTTP_DELETE, handleDeleteSchedule);
  server.on("/api/schedules/delete", HTTP_OPTIONS, handleCORS);

  // Ruta de prueba
  server.on("/", HTTP_GET, handleRoot);
}

// ========== HANDLERS DE DASHBOARD ==========

void sendCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
  server.sendHeader("Access-Control-Max-Age", "86400");
}

void handleCORS() {
  sendCORSHeaders();
  server.send(204);
}

void handleGetFullness() {
  sendCORSHeaders();
  StaticJsonDocument<200> doc;

  fullnessPercentage = get_tank_level();

  doc["percentage"] = fullnessPercentage;

  if (fullnessPercentage >= 75) {
    doc["status"] = "Alto";
  } else if (fullnessPercentage >= 50) {
    doc["status"] = "Medio";
  } else {
    doc["status"] = "Bajo";
  }

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
}

void handleGetRequests() {
  sendCORSHeaders();
  DynamicJsonDocument doc(2048);
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < requestCount; i++) {
    JsonObject obj = array.createNestedObject();
    obj["hora"] = requests[i].hora;
    obj["estado"] = requests[i].estado;
  }

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
}

void handleCompleteRequest() {
  sendCORSHeaders();
  if (server.hasArg("plain")) {
    String body = server.arg("plain");

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    String hora = doc["hora"];

    // Buscar y actualizar la solicitud
    for (int i = 0; i < requestCount; i++) {
      if (requests[i].hora == hora) {
        requests[i].estado = "Completed";
        server.send(200, "application/json", "{\"message\":\"Request completed\"}");
        return;
      }
    }

    server.send(404, "application/json", "{\"error\":\"Request not found\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleCancelRequest() {
  sendCORSHeaders();
  if (server.hasArg("plain")) {
    String body = server.arg("plain");

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    String hora = doc["hora"];

    // Buscar y actualizar la solicitud
    for (int i = 0; i < requestCount; i++) {
      if (requests[i].hora == hora) {
        requests[i].estado = "Cancelled";
        server.send(200, "application/json", "{\"message\":\"Request cancelled\"}");
        return;
      }
    }

    server.send(404, "application/json", "{\"error\":\"Request not found\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleRefill() {
  sendCORSHeaders();
  //fullnessPercentage = fullnessPercentage - 5;
  fullnessPercentage = get_tank_level();

  StaticJsonDocument<200> doc;
  doc["message"] = "Bowl refilled successfully";
  doc["percentage"] = fullnessPercentage;

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
}

// ========== HANDLERS DE HORARIOS ==========

void handleGetSchedules() {
  sendCORSHeaders();
  DynamicJsonDocument doc(2048);
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < scheduleCount; i++) {
    JsonObject obj = array.createNestedObject();
    obj["id"] = schedules[i].id;
    obj["time"] = schedules[i].time;
    obj["active"] = schedules[i].active;
  }

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
  print_schedules();
}

void handleCreateSchedule() {
  sendCORSHeaders();
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    if (scheduleCount >= 10) {
      server.send(400, "application/json", "{\"error\":\"Maximum schedules reached\"}");
      return;
    }

    // Crear nuevo horario
    schedules[scheduleCount].id = scheduleCount + 1;
    schedules[scheduleCount].time = doc["time"].as<String>();
    schedules[scheduleCount].active = doc["active"];
    // Added to configure interrupt handler
    schedules[scheduleCount].hour = schedules[scheduleCount].time.substring(0, 2).toInt();
    schedules[scheduleCount].minute = schedules[scheduleCount].time.substring(3, 5).toInt();
    schedules[scheduleCount].func = dispense_food;
    schedules[scheduleCount].triggered = false;

    scheduleCount++;

    StaticJsonDocument<200> response;
    response["message"] = "Schedule created successfully";
    response["id"] = scheduleCount;

    String responseStr;
    serializeJson(response, responseStr);

    server.send(201, "application/json", responseStr);
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }

  print_schedules();
}

void handleUpdateSchedule() {
  sendCORSHeaders();
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    int id = doc["id"];

    // Buscar y actualizar el horario
    for (int i = 0; i < scheduleCount; i++) {
      if (schedules[i].id == id) {
        if (doc.containsKey("time")) {
          schedules[i].time = doc["time"].as<String>();
          // Added to configure interrupt handler
          schedules[i].hour = schedules[i].time.substring(0, 2).toInt();
          schedules[i].minute = schedules[i].time.substring(3, 5).toInt();
        }
        if (doc.containsKey("active")) {
          schedules[i].active = doc["active"];
          schedules[i].triggered = false;
        }

        server.send(200, "application/json", "{\"message\":\"Schedule updated\"}");
        return;
      }
    }

    server.send(404, "application/json", "{\"error\":\"Schedule not found\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }

  print_schedules();
}

void handleDeleteSchedule() {
  sendCORSHeaders();
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    int id = doc["id"];

    // Buscar y eliminar el horario
    for (int i = 0; i < scheduleCount; i++) {
      if (schedules[i].id == id) {
        // Mover todos los elementos siguientes una posición atrás
        for (int j = i; j < scheduleCount - 1; j++) {
          schedules[j] = schedules[j + 1];
        }
        scheduleCount--;

        server.send(200, "application/json", "{\"message\":\"Schedule deleted\"}");
        return;
      }
    }

    server.send(404, "application/json", "{\"error\":\"Schedule not found\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

// ========== HANDLERS AUXILIARES ==========

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>FoodDPet API</h1>";
  html += "<p>API REST funcionando correctamente</p>";
  html += "<h2>Endpoints disponibles:</h2>";
  html += "<ul>";
  html += "<li>GET /api/fullness - Obtener estado de llenado</li>";
  html += "<li>GET /api/requests - Obtener solicitudes</li>";
  html += "<li>POST /api/requests/complete - Completar solicitud</li>";
  html += "<li>POST /api/requests/cancel - Cancelar solicitud</li>";
  html += "<li>POST /api/refill - Rellenar dispensador</li>";
  html += "<li>GET /api/schedules - Obtener horarios</li>";
  html += "<li>POST /api/schedules - Crear horario</li>";
  html += "<li>PUT /api/schedules/update - Actualizar horario</li>";
  html += "<li>DELETE /api/schedules/delete - Eliminar horario</li>";
  html += "</ul>";
  html += "<p>IP: " + WiFi.localIP().toString() + "</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleNotFound() {
  sendCORSHeaders();
  server.send(404, "application/json", "{\"error\":\"Endpoint not found\"}");
}

int measure_distance(int sensor) {
  int distance;
  // Laser sensor
  if (sensor == 1) {
    bool start = vl53.startRanging();
    if (start) {
      if (vl53.dataReady()) {
        // new measurement for the taking!
        distance = vl53.distance();
        if (distance == -1) {
          // something went wrong!
          Serial.print(F("No se pudo obtener distancia: "));
          Serial.println(vl53.vl_status);
          return -1;
        } else {
          // data is read out, time for another reading!
          vl53.clearInterrupt();
          vl53.stopRanging();
          return distance;
        }
      }
    } else {
      return -1;
    }
  } else {
    // Ultrasonic sensor
    // Clears the trigPin
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    long duration;
    duration = pulseIn(ECHO_PIN, HIGH);

    // Calculating the distance
    distance = duration * 0.034 / 2;

    // Prints the distance on the Serial Monitor
    Serial.print(String(distance));
    Serial.println(" cm");
    return distance;
  }
}


int get_tank_level() {
  int distance = measure_distance(1);
  Serial.println(distance);
  if (distance < 0) {
    Serial.println("No se pudo tomar medida");
    return -1;
  } else if (distance >= 0 && distance < 50) {
    Serial.println("Full tank");
    return 100;
  } else if (distance >= 50 && distance < 100) {
    Serial.println("Almost full tank");
    return 75;
  } else if (distance >= 100 && distance < 120) {
    Serial.println("Half full tank");
    return 50;
  } else if (distance >= 120 && distance < 140) {
    Serial.println("Almost empty tank");
    return 25;
  } else {
    Serial.println("Empty tank");
    return 0;
  }
}


// ------------------- Interrupciones -----------------------------

void check_events() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No se pudo obtener la hora...");
    return;
  }

  // Mostrar la hora actual
  //Serial.printf("Hora actual: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  for (int i = 0; i < scheduleCount; i++) {

    // comparar hora actual y hora del evento
    if (timeinfo.tm_hour == schedules[i].hour && timeinfo.tm_min == schedules[i].minute) {

      if (!schedules[i].triggered && schedules[i].active) {
        Serial.println("Ejecutando evento ...");
        schedules[i].func();
        schedules[i].triggered = true;
      }

    } else {
      schedules[i].triggered = false;
    }
  }
}

void print_schedules() {
  Serial.println("------ Eventos --------");
  for (int i = 0; i < scheduleCount; i++) {
    Serial.print("ID: ");
    Serial.println(schedules[i].id);
    Serial.print("Time: ");
    Serial.println(schedules[i].time);
    Serial.print("Hour: ");
    Serial.println(schedules[i].hour);
    Serial.print("Minute: ");
    Serial.println(schedules[i].minute);
    Serial.print("Active: ");
    Serial.println(schedules[i].active);
    Serial.print("Triggered: ");
    Serial.println(schedules[i].triggered);
    Serial.println("---------------------");
  }
}

void dispense_food() {
  Serial.println(">>> ¡Servir comida! <<<");
  // En este método tiene que ir el control del motor
}