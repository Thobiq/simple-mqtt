#include <Bonezegei_DHT11.h>
#include <WiFi.h>
#include <PubSubClient.h>

// wifi
const char *ssid = "SIDAH";
const char *password = "shofi1508";

// mqtt broker
const char *mqtt_broker = "broker.tbqproject.my.id";
const char *topic = "tbq/broker";
const char *mqtt_username = "tbqmqtt";
const char *mqtt_password = "tbq2412";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

Bonezegei_DHT11 dht(16);

// setup wifi
void setup_wifi(){
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wifi Terhubung IP : " + String(WiFi.localIP()));

}


void setup() {
  Serial.begin(9600);
  setup_wifi();
  dht.begin();

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
  }

  // client.publish(topic, "Hi, I'm ESP32 ^^");
  // client.subscribe(topic);

}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {

  client.loop();

  if(dht.getData()){
    float tempCelc = dht.getTemperature();
    float hum = dht.getHumidity();

    String msgTemp = "Suhu : ";
           msgTemp += String(tempCelc);
    
    String msgHum = "Kelembaban : ";
           msgHum += String(hum);

    client.publish(topic, msgTemp.c_str());
    client.publish(topic, msgHum.c_str());

    Serial.println(msgTemp);
    Serial.println(msgHum);
  }

  delay(500);
}
