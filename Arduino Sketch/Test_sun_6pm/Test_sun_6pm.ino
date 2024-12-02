#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

// Our WiFi and MQTT credentials
const char* ssid          = "your_SSID";
const char* password      = "your_PASSWORD";
const char* mqtt_username = "your_MQTT_USERNAME";
const char* mqtt_password = "your_MQTT_PASSWORD";
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
const char* wifiHostname = "Lumina_ucbqmie";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const int numLDRs = 6; 
const int threshold = 20; // threshold value for the LDRs
int ldrPins[numLDRs] = {A0, A1, A2, A3, A4, A5};
int ldrValues[numLDRs]; // this will hold an array of the values that will be read from each LDR
int ldrArrays[numLDRs][6] = { //this is a 2d array in which each row represents a group of LED numbers associated with a particular LDR.
    {4, 5, 7, 8, 11, 12},
    {11, 12, 15, 16, 19, 20},
    {14, 15, 18, 19, 22, 26},
    {17, 18, 21, 22, 26, 27},
    {19, 20, 23, 24, 28, 29},
    {24, 25, 29, 30, 34, 35}
};

void setup() {

  
  Serial.begin(115200);
  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
  connectToMQTT();

  for (int i = 0; i < numLDRs; i++) { //defining each LDR as an Input
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
    ldrValues[i] = constrain(ldrValues[i], 20, 200); // Clamp values to the range 20-200
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
  char topic[50];
  char payload[100];
  
  for (int i = 0; i < numLDRs; i++) {
    if (ldrValues[i] > threshold) {
      int R = map(ldrValues[i], 20, 200, 0, 255);
      int G = map(ldrValues[i], 20, 200, 255, 0);
      int B = map(ldrValues[i], 20, 200, 255, 0);
      int W = 100; // Set white value to 100
      for (int j = 0; j < 6; j++) {
        int ledValue = ldrArrays[i][j];
        sprintf(topic, "student/CASA0014/light/%d/pixel/", ledValue);
        Serial.println(topic);
        for (int k = 0; k <= 12; k++) {
          if (ledValue == 4 || ledValue == 14 || ledValue == 16 || ledValue == 17 || ledValue == 25 || ledValue == 28) {
            sprintf(payload, "{\"pixelid\": %d, \"R\": 0, \"G\": 0, \"B\": 0, \"W\": 60}", k);
          } else {
            sprintf(payload, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}", k, R, G, B, W);
          }
          Serial.println(payload);
          client.publish(topic, payload);
        }
      }
    }
  }
}