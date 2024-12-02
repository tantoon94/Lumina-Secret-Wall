#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiServer server(80);
WiFiClient wificlient;

WiFiClient mkrClient;
PubSubClient client(mkrClient);

const int numLDRs = 6;
const int threshold = 40; // here we define threshold value for LDRs
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
  // Start the serial monitor to show output
  Serial.begin(115200);
  delay(1000);

  WiFi.setHostname("Lumina ucbqmie");
  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("setup complete");
}

void loop() {
  // Ensure the Arduino is still connected to the MQTT broker
  if (!client.connected()) {
    reconnectMQTT();
  }

  // Ensure we are connected to the WiFi
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // Check for messages from the broker and ensure outgoing messages are sent
  client.loop();

  readLDRValues();
  sendmqtt();

  Serial.println("sent a message");
  delay(10000);
}

void readLDRValues() {
  for (int i = 0; i < numLDRs; i++) {
    ldrValues[i] = analogRead(ldrPins[i]);
    Serial.print("LDR ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(ldrValues[i]);
  }
}

void sendmqtt() {
  char topic[50];
  char payload[100];

  for (int i = 0; i < numLDRs; i++) {
    if (ldrValues[i] > threshold) {
      int cappedValue = constrain(ldrValues[i], 20, 200);
      int R = map(cappedValue, 20, 200, 0, 255);
      int G = map(cappedValue, 20, 200, 255, 0);
      int B = map(cappedValue, 20, 200, 255, 0);
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
    } else {
      for (int j = 0; j < 6; j++) {
        int ledValue = ldrArrays[i][j];
        sprintf(topic, "student/CASA0014/light/%d/pixel/", ledValue);
        Serial.println(topic);
        for (int k = 0; k <= 12; k++) {
          sprintf(payload, "{\"pixelid\": %d, \"R\": 0, \"G\": 0, \"B\": 0, \"W\": 0}", k);
          Serial.println(payload);
          client.publish(topic, payload);
        }
      }
    }
  }
}

void startWifi() {
  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true); // Don't continue
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Function for connecting to a WiFi network
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.print("Trying to connect to: ");
    Serial.println(ssid);
    for (int i = 0; i < n; ++i) {
      String availablessid = WiFi.SSID(i);
      if (availablessid.equals(ssid)) {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
          delay(600);
          Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("Connected to " + String(ssid));
          break; // Exit the loop if connected
        } else {
          Serial.println("Failed to connect to " + String(ssid));
        }
      } else {
        Serial.print(availablessid);
        Serial.println(" - this network is not in my list");
      }
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, int length) {
  // Handle incoming messages
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}