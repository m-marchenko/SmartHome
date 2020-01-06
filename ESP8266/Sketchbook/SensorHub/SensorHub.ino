#include "WiFiConfig.h"
#include "HttpClient.h"
#include "ESP-nRFConfig.h"

#include <SPI.h>

const char* ssid = "WatchOut!";
const char* password = "djctvmgjgeufq";
//const char* ssid = "MegaFon_MR100-1";
//const char* password = "95955870";


const char * _smart_url = "http://smart.no-troubles.com/Command/SetSensorValue";
//const char * _smart_url = "http://192.168.10.109/SmartHome/Command/SetSensorValue";

const uint64_t _pipe = 0xC0C0C0C049;

tpacket packet;

RF24 radio(4,16);

void setup() {

  Serial.begin(9600);

  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);   
 
  setup_wifi(ssid, password);
  
  setup_radio(radio, _pipe);

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

      Serial.print("Temperature: ");
      if (packet.buf[0]) Serial.print("-");
      float t = packet.buf[1] / 2.0;

      String msg = String(packet.heart_beat) + String(" Temperature: ") + String((packet.buf[0])?'-':' ') + String(t);      
      
      //String mqstring = String("{\"clientid\":\"root_house_firstfloor\", \"display\":\"testing temperature sensor\", \"value\":\"") + String((packet.buf[0])?'-':' ') + String(t) + String("\", \"time\":\"") + String(packet.heart_beat) + String("\"}");      
      //client.publish(topic, mqstring.c_str());

      String sensor_id = String("TM000") + String(packet.clid);
      String post_string = "{\"sensorId\":\"" + sensor_id + "\", \"val\":\"" + String((packet.buf[0])?'-':' ') + String(t, 1) + "\" }";      
      post_message(_smart_url, post_string);
      
      Serial.println(t);
      
      Serial.print("Buffer: "); 
      for (int i = 0; i < sizeof(packet.buf); i++)
      {
        String str = String(packet.buf[i], HEX) + " ";
        Serial.print(str);
      }
            
      Serial.println();
      Serial.print("Sending: ");
      Serial.println(post_string);
      Serial.println();
      //delay(1000);

      digitalWrite(GPIO2, HIGH);
    }  


}
