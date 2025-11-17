/*
 * FoodDPet API REST para ESP32
 * API para gestionar horarios de alimentación y solicitudes
 */

#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);

#include <ArduinoJson.h>

// Configuración WiFi
const char *ssid = "CHANGE_YOUR_SSID";
const char *password = "CHANGE_YOUR_PASSWORD";

// Estructura para Solicitudes
struct FoodRequest
{
    String hora;
    String estado; // "Completed", "Pending", "Cancelled"
};

// Estructura para Horarios
struct Schedule
{
    int id;
    String time;
    bool active;
};

// Datos de prueba - Solicitudes
FoodRequest requests[10];
int requestCount = 4;

// Datos de prueba - Horarios
Schedule schedules[10];
int scheduleCount = 3;

// Estado del dispensador
int fullnessPercentage = 75;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Inicializar datos de prueba
    initializeTestData();

    // Conectar a WiFi
    Serial.println("\nConectando a WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n¡WiFi conectado!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configurar rutas de la API
    setupRoutes();

    // Iniciar servidor
    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

void loop()
{
    server.handleClient();
}

void initializeTestData()
{
    // Inicializar solicitudes de prueba
    requests[0] = {"2024-11-15T10:00:00", "Pending"};
    requests[1] = {"2024-11-15T14:30:00", "Cancelled"};
    requests[2] = {"2024-11-15T09:15:00", "Completed"};
    requests[3] = {"2024-11-15T16:45:00", "Pending"};

    // Inicializar horarios de prueba
    schedules[0] = {1, "08:00", true};
    schedules[1] = {2, "14:00", true};
    schedules[2] = {3, "20:00", false};
}

void setupRoutes()
{
    // Handler para todas las solicitudes OPTIONS (CORS preflight)
    server.onNotFound([]()
                      {
        if (server.method() == HTTP_OPTIONS) {
            handleCORS();
        } else {
            handleNotFound();
        } });

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

void sendCORSHeaders()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.sendHeader("Access-Control-Max-Age", "86400");
}

void handleCORS()
{
    sendCORSHeaders();
    server.send(204);
}

void handleGetFullness()
{
    sendCORSHeaders();
    StaticJsonDocument<200> doc;

    doc["percentage"] = fullnessPercentage;

    if (fullnessPercentage >= 75)
    {
        doc["status"] = "Alto";
    }
    else if (fullnessPercentage >= 50)
    {
        doc["status"] = "Medio";
    }
    else
    {
        doc["status"] = "Bajo";
    }

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}

void handleGetRequests()
{
    sendCORSHeaders();
    DynamicJsonDocument doc(2048);
    JsonArray array = doc.to<JsonArray>();

    for (int i = 0; i < requestCount; i++)
    {
        JsonObject obj = array.createNestedObject();
        obj["hora"] = requests[i].hora;
        obj["estado"] = requests[i].estado;
    }

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}

void handleCompleteRequest()
{
    sendCORSHeaders();
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String hora = doc["hora"];

        // Buscar y actualizar la solicitud
        for (int i = 0; i < requestCount; i++)
        {
            if (requests[i].hora == hora)
            {
                requests[i].estado = "Completed";
                server.send(200, "application/json", "{\"message\":\"Request completed\"}");
                return;
            }
        }

        server.send(404, "application/json", "{\"error\":\"Request not found\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"No data received\"}");
    }
}

void handleCancelRequest()
{
    sendCORSHeaders();
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        String hora = doc["hora"];

        // Buscar y actualizar la solicitud
        for (int i = 0; i < requestCount; i++)
        {
            if (requests[i].hora == hora)
            {
                requests[i].estado = "Cancelled";
                server.send(200, "application/json", "{\"message\":\"Request cancelled\"}");
                return;
            }
        }

        server.send(404, "application/json", "{\"error\":\"Request not found\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"No data received\"}");
    }
}

void handleRefill()
{
    sendCORSHeaders();
    fullnessPercentage = fullnessPercentage - 5;

    StaticJsonDocument<200> doc;
    doc["message"] = "Bowl refilled successfully";
    doc["percentage"] = fullnessPercentage;

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}

// ========== HANDLERS DE HORARIOS ==========

void handleGetSchedules()
{
    sendCORSHeaders();
    DynamicJsonDocument doc(2048);
    JsonArray array = doc.to<JsonArray>();

    for (int i = 0; i < scheduleCount; i++)
    {
        JsonObject obj = array.createNestedObject();
        obj["id"] = schedules[i].id;
        obj["time"] = schedules[i].time;
        obj["active"] = schedules[i].active;
    }

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}

void handleCreateSchedule()
{
    sendCORSHeaders();
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        StaticJsonDocument<300> doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        if (scheduleCount >= 10)
        {
            server.send(400, "application/json", "{\"error\":\"Maximum schedules reached\"}");
            return;
        }

        // Crear nuevo horario
        schedules[scheduleCount].id = scheduleCount + 1;
        schedules[scheduleCount].time = doc["time"].as<String>();
        schedules[scheduleCount].active = doc["active"];

        scheduleCount++;

        StaticJsonDocument<200> response;
        response["message"] = "Schedule created successfully";
        response["id"] = scheduleCount;

        String responseStr;
        serializeJson(response, responseStr);

        server.send(201, "application/json", responseStr);
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"No data received\"}");
    }
}

void handleUpdateSchedule()
{
    sendCORSHeaders();
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        StaticJsonDocument<300> doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        int id = doc["id"];

        // Buscar y actualizar el horario
        for (int i = 0; i < scheduleCount; i++)
        {
            if (schedules[i].id == id)
            {
                if (doc.containsKey("time"))
                {
                    schedules[i].time = doc["time"].as<String>();
                }
                if (doc.containsKey("active"))
                {
                    schedules[i].active = doc["active"];
                }

                server.send(200, "application/json", "{\"message\":\"Schedule updated\"}");
                return;
            }
        }

        server.send(404, "application/json", "{\"error\":\"Schedule not found\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"No data received\"}");
    }
}

void handleDeleteSchedule()
{
    sendCORSHeaders();
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error)
        {
            server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
            return;
        }

        int id = doc["id"];

        // Buscar y eliminar el horario
        for (int i = 0; i < scheduleCount; i++)
        {
            if (schedules[i].id == id)
            {
                // Mover todos los elementos siguientes una posición atrás
                for (int j = i; j < scheduleCount - 1; j++)
                {
                    schedules[j] = schedules[j + 1];
                }
                scheduleCount--;

                server.send(200, "application/json", "{\"message\":\"Schedule deleted\"}");
                return;
            }
        }

        server.send(404, "application/json", "{\"error\":\"Schedule not found\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"No data received\"}");
    }
}

// ========== HANDLERS AUXILIARES ==========

void handleRoot()
{
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

void handleNotFound()
{
    sendCORSHeaders();
    server.send(404, "application/json", "{\"error\":\"Endpoint not found\"}");
}
