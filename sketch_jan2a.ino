#include <Bonezegei_DHT11.h>
#include <WiFi.h>
#include <PubSubClient.h>

// wifi
const char *ssid = "Koshijau";
const char *password = "Piay1210";

// mqtt broker
const char *mqtt_broker = "8.215.29.171";
const char *topic = "tbq/broker";
const char *mqtt_username = "tbqmqtt";
const char *mqtt_password = "tbq2412";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

Bonezegei_DHT11 dht(4);

// interval untuk publish data
unsigned long lastPublish = 0;
const long publishInterval = 5000; // setiap 5 detik

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wifi Terhubung IP : " + String(WiFi.localIP()));
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker.");
      client.subscribe(topic); // optional, kalau mau terima pesan juga
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  dht.begin();

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - lastPublish > publishInterval) {
    if (dht.getData()) {
      float temp = dht.getTemperature();
      float hum = dht.getHumidity();

      String msgTemp = "Suhu : " + String(temp);
      String msgHum = "Kelembaban : " + String(hum);

      client.publish(topic, msgTemp.c_str());
      client.publish(topic, msgHum.c_str());

      Serial.println(msgTemp);
      Serial.println(msgHum);
    }
    lastPublish = millis();
  }
}
