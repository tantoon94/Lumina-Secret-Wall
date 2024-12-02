#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h" 

// WiFi and MQTT credentials
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
const char* wifiHostname = "Lumina ucbqmie";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const int numLDRs = 6;
const int threshold = 100; // Adjust this threshold as needed
int ldrPins[numLDRs] = {A0, A1, A2, A3, A4, A5};
int ldrValues[numLDRs];
int ldrArrays[numLDRs][6] = {
    {4, 5, 7, 8, 11, 12},
    {11, 12, 15, 16, 19, 20},
    {14, 15, 18, 19, 22, 26},
    {17, 18, 21, 22, 26, 27},
    {19, 20, 23, 24, 28, 29},
    {24, 25, 29, 30, 34, 35}
};

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
  connectToMQTT();
  for (int i = 0; i < numLDRs; i++) {
    pinMode(ldrPins[i], INPUT);
  }
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  for (int i = 0; i < numLDRs; i++) {
    ldrValues[i] = analogRead(ldrPins[i]);
  }
  printLDRValues();
  printLDRStatus();
  publishLDRMessages();
  delay(1000); // Delay for readability
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.setHostname(wifiHostname);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(wifiHostname, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void printLDRValues() {
  for (int i = 0; i < numLDRs; i++) {
    Serial.print("A");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(ldrValues[i]);
    if (i < numLDRs - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
}

void printLDRStatus() {
  for (int i = 0; i < numLDRs; i++) {
    if (ldrValues[i] > threshold) {
      Serial.print("A");
      Serial.print(i);
      Serial.print(": ON");
    } else {
      Serial.print("A");
      Serial.print(i);
      Serial.print(": OFF");
    }
    if (i < numLDRs - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
}

void publishLDRMessages() {
  for (int i = 0; i < numLDRs; i++) {
    if (ldrValues[i] > threshold) {
      for (int j = 0; j < 6; j++) {
        String topic = "student/CASA0014/light/" + String(ldrArrays[i][j]) + "/pixel/";
        Serial.println(topic);
        for (int k = 0; k <= 12; k++) {
          String payload = String("{\"pixelid\": ") + String(k) + ", \"R\": 255, \"G\": 0, \"B\": 0, \"W\": 0}";
          Serial.println(payload);
          client.publish(topic.c_str(), payload.c_str());
        }
      }
    }
  }
}