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

WiFiClient mkrClient;
PubSubClient client(mkrClient);
char mqtt_topic_demo[] = "student/CASA0014/light/16/All/";

void setup() {
  // Start the serial monitor to show output
  Serial.begin(115200);
  delay(1000);

  WiFi.setHostname("Lumina ucbqmie");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  client.setServer(mqtt_server, mqtt_port);
  Serial.println("setup complete");
}

void loop() {
  // Ensure the Arduino is connected to the MQTT broker
  if (!client.connected()) {
    Serial.println("Client is disconnected...try to connect in 5 sec");
    if (client.connect("Lumina ucbqmie", mqtt_username, mqtt_password)) {
      Serial.println("Client connected");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.println(client.state());
      delay(5000);
      return; // Exit the loop function if not connected
    }
  }

  // Ensure we are connected to the WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost WiFi Connection...try to reconnect in 5 sec");
    delay(5000);
    WiFi.begin(ssid, password);
    return; // Exit the loop function if not connected
  }

  // Check for messages from the broker and ensure that any outgoing messages are sent.
  client.loop();

  sendmqtt();

  Serial.println("sent a message");
  delay(5000);
}

void sendmqtt() {
  // Define the buffer size
  const int bufferSize = 512;
  char mqtt_message[bufferSize]; // Adjust size as needed
  char mqtt_topic_demo[] = "student/CASA0014/light/16/all/";

  // Start the JSON payload
  strcpy(mqtt_message, "{\"allLEDs\":[");

  for (int pixelid = 0; pixelid < 12; pixelid++) {
    char pixel_message[100];
    sprintf(pixel_message, "{\"pixelid\": %d, \"R\": %d, \"G\": %d, \"B\": %d, \"W\": %d}", pixelid, 150, 150, 150, 200);
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
      if (client.publish(mqtt_topic_demo, mqtt_message)) {
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