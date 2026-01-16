#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>

const uint64_t chipid = ESP.getEfuseMac();

const char* ssid = "__SSID__";
const char* password = "__PASSWORD__";

const char* mqtt_server = "__MQTT_SERVER__";
const int mqtt_port = 1883;
const char* mqtt_user = "__MQTT_USER__";
const char* mqtt_pass = "__MQTT_PASS__";

const char* mqtt_client_id = "esp32_pod";

String device_id_str;
String mqtt_ping_topic_str;

const char* mqtt_ping_topic;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastSend = 0;
unsigned long counter = 0;

void reconnect() {
  Serial.println("Reconnect to mqtt server");
  while (!client.connected()) {
    Serial.println("Try to connect to the mqtt server...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("Connected to the mqtt server");
    } else {
      Serial.print("MQTT connect failed, state=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

String getValueString() {
  Serial.println("Get value string");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    counter++;
    Serial.println("Returned with the fallback value");
    return String(counter);
  }

  counter = 0;
  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  Serial.println("Returned with the time string");
  return String(buffer);
}

void setup() {
  Serial.begin(115200);

  Serial.println("Setup variables");
  device_id_str = "esp32/" + String((uint32_t)chipid, HEX);
  device_id_str.toLowerCase();

  mqtt_ping_topic_str = device_id_str + "/ping";
  mqtt_ping_topic = mqtt_ping_topic_str.c_str();

  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("Config time");
  configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");

  Serial.println("Set mqtt server");
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    Serial.println(client.state());
    reconnect();
  }
  client.loop();

  if (millis() - lastSend > 10000) {
    Serial.println("Send mqtt message");
    lastSend = millis();

    String msg = getValueString();

    client.publish(mqtt_ping_topic, msg.c_str());
    Serial.println("Sent: " + msg);
  }
}