#include "WiFiConfig.h"
//#include "HttpClient.h"
#include "ESP-nRFConfig.h"
#include "MqttClient.h"

#include <SPI.h>

const char* ssid = "WatchOut!";
const char* password = "djctvmgjgeufq";
//const char* ssid = "MegaFon_MR100-1";
//const char* password = "95955870";

const char * mqtt_server = "m10.cloudmqtt.com";
const char * mqtt_user = "quefkobd";
const char * mqtt_password = "524rVRkbylHl";
const char * mqtt_topic = "root/sensors";
#define MQTT_PORT 16425

const char * _smart_url = "http://smart.no-troubles.com/Command/SetSensorValue";
//const char * _smart_url = "http://192.168.10.109/SmartHome/Command/SetSensorValue";

const uint64_t _pipe = 0xC0C0C0C049;

tpacket packet;

RF24 radio(4,16);

WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {

  Serial.begin(9600);

  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);   
 
  setup_wifi(ssid, password);
  
  setup_radio(radio, _pipe);

  setup_mqtt(client, mqtt_server, MQTT_PORT, mqtt_callback);

  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO5, OUTPUT);   
  digitalWrite(GPIO5, HIGH);
  digitalWrite(GPIO2, HIGH);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(GPIO5, HIGH);
    setup_wifi(ssid, password);
  }
  else {
    digitalWrite(GPIO5, LOW);
  }
  
  if (!client.connected()) {
    reconnect_mqtt(client, mqtt_user, mqtt_password);
  }
  client.loop();
  
  if(radio.available())
  {
      digitalWrite(GPIO2, LOW);
      
      uint8_t len = radio.getDynamicPayloadSize();      
      radio.read((void *)&packet, len);      
      
      Serial.print("Bytes recieved: "); Serial.println(len);
      Serial.print("Packets lost: "); Serial.println(packet.lost_packet_count);
      Serial.print("Counter: "); Serial.println(packet.heart_beat);
      Serial.print("Status: "); Serial.println("0x" + String(packet.status, HEX));
      Serial.print("Transmitter no: "); Serial.println(packet.clid);

      float t = packet.buf[1] / 2.0;
      Serial.print("Temperature: ");
      if (packet.buf[0]) {      
        Serial.print("-");
      }            
      Serial.println(t);
      
      String sensor_id = String("TM000") + String(packet.clid);
      String post_string = "{\"sensorId\":\"" + sensor_id + "\", \"val\":\"" + String((packet.buf[0])?'-':' ') + String(t, 1) + "\" }";      
      //post_message(_smart_url, post_string);

      client.publish(mqtt_topic, post_string.c_str());
                  
      Serial.print("Buffer: "); 
      for (int i = 0; i < sizeof(packet.buf); i++)
      {
        String str = String(packet.buf[i], HEX) + " ";
        Serial.print(str);
      }
            
      Serial.println();
      Serial.print("Message published: ");
      Serial.println(post_string);
      Serial.println();
      //delay(1000);

      digitalWrite(GPIO2, HIGH);
  }  
}
