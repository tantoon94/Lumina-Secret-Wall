#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h" 

/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
 */

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

const int numSensors = 6; // Number of sensors
const int sensorPins[numSensors] = {A0, A1, A2, A3, A4, A5}; // Sensor pins
const int thresholds[numSensors] = {100, 150, 200, 250, 300, 350}; // Thresholds for each sensor

WiFiClient mkrClient;
PubSubClient client(mkrClient);

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
  // Ensure the Arduino is connected to the MQTT broker
  if (!client.connected()) {
    reconnectMQTT();
  }

  // Ensure we are connected to the WiFi
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }

  // Check for messages from the broker and ensure that any outgoing messages are sent.
  client.loop();

  sendmqtt();

  Serial.println("sent a message");
  delay(1000);
}

void sendmqtt() {
  String sensorValues = ""; // String to hold all sensor values
  String ledStatus = ""; // String to hold LED status

  for (int i = 0; i < numSensors; i++) {
    int sensorValue = analogRead(sensorPins[i]); // Read the sensor value

    // Append sensor value to the string
    sensorValues += "A" + String(i) + ": " + String(sensorValue) + " ";

    // Control the corresponding LEDs and append LED status to the string
    ledStatus += controlLEDs(i, sensorValue);
  }

  // Print all sensor values in a single line
  Serial.println(sensorValues);

  // Print all LED statuses in a single line
  Serial.println(ledStatus);
}

String controlLEDs(int sensorIndex, int sensorValue) {
  // Define which LEDs each sensor controls
  const int ledMapping[6][6] = {
    {4, 5, 7, 8, 11, 12}, // A0
    {11, 12, 15, 16, 19, 20}, // A1
    {14, 15, 18, 19, 22, 26}, // A2
    {17, 18, 21, 22, 26, 27}, // A3
    {19, 20, 23, 24, 28, 29}, // A4
    {24, 25, 29, 30, 34, 35} // A5
  };

  String ledStatus = "A" + String(sensorIndex) + ": ";

  for (int j = 0; j < 6; j++) {
    int ledIndex = ledMapping[sensorIndex][j];
    if (sensorValue > thresholds[sensorIndex]) {
      // Define the buffer size
      const int bufferSize = 2048; // Adjust size as needed
      char mqtt_message[bufferSize];
      String mqtt_topic_demo = "student/CASA0014/light/" + String(ledIndex) + "/pixel/";

      // Start the JSON payload
      //strcpy(mqtt_message, "{\"allLEDs\":[");

      for (int pixelid = 0; pixelid < 12; pixelid++) {
        char pixel_message[100];
        sprintf(pixel_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}", pixelid, 255, 0, 0, 0);
        strcat(mqtt_message, pixel_message);
        if (pixelid < 11) strcat(mqtt_message, ","); // Add comma between JSON objects
      }

      // End the JSON payload
      strcat(mqtt_message, "]}");

      // Calculate the length of the message
      int messageLength = strlen(mqtt_message);

      // Print the topic and message for debugging
      Serial.println(mqtt_topic_demo);
      Serial.println(mqtt_message);

      // Check if the message length is within the buffer size
      if (messageLength < bufferSize) {
        if (client.connected()) {
          if (client.publish(mqtt_topic_demo.c_str(), mqtt_message)) {
            Serial.println("Message published");
          } else {
            Serial.println("Failed to publish message");
          }
        } else {
          Serial.println("MQTT client not connected");
        }
      } else {
        Serial.println("Message size exceeds buffer size");
      }
    }
  }

  return ledStatus;
}

void startWifi() {
  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // Don't continue
    while (true);
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
      // Primary network
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
    // Create a random client ID
    String clientId = "LuminaSelector";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
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