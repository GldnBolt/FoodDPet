#include "WiFiEsp.h"
#include "Adafruit_VL53L1X.h"
#include "SoftwareSerial.h"

#define IRQ_PIN 2
#define XSHUT_PIN 3

#define TRIGGER_PIN 11
#define ECHO_PIN 12

#define RX 6
#define TX 7

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

char ssid[] = "Familia Martinez 2.4";  // your network SSID (name)
char pass[] = "JimmyAntonio";          // your network password
int status = WL_IDLE_STATUS;           // the Wifi radio's status

char server[] = "api.ipify.org";

// Initialize the Ethernet client object
WiFiEspClient client;
SoftwareSerial Serial1(RX, TX);  // RX, TX

void setup_wifi() {
  // initialize ESP module
  WiFi.init(&Serial1);

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    // Hacer un request HTTP GET válido
    client.println("GET /?format=json HTTP/1.1");
    client.println("Host: api.ipify.org");
    client.println("Connection: close");
    client.println();
  }
}

void setup_sensors() {
  // Ultrasonic sensor pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Laser sensor
  Wire.begin();
  if (!vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);
}

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  while (!Serial) delay(10);
  // setup both distance sensors
  setup_sensors();

  // initialize serial for ESP module
  Serial1.begin(115200);
  while (!Serial1) delay(10);
  // set up wifi connection
  setup_wifi();
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
          Serial.print(F("Couldn't get distance: "));
          Serial.println(vl53.vl_status);
          return;
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
    Serial.println("Could not take measurement");
    return -1;
  } else if (distance >= 0 && distance < 100) {
    Serial.println("Full tank");
    return 1;
  } else if (distance >= 100 && distance < 120) {
    Serial.println("Half full tank");
    return 2;
  } else if (distance >= 120 && distance < 140) {
    Serial.println("Almost empty tank");
    return 3;
  } else {
    Serial.println("Empty tank");
    return 4;
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    while (Serial.available() == 0) {}

    String cmd = Serial.readString();

    cmd.trim();  // remove any \r \n whitespace at the end of the String

    if (cmd == "1") {
      int level = get_tank_level();
      // Send to server
    } else if (cmd == "2") {
      int proximity = measure_distance(2);
    } else {
      Serial.println("Unsupported command");
    }
  }
}
