#include <string.h>
#include <stdlib.h>

#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <SPI.h>

//#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

//nRF24L01 - ESP8266 12
//1 GRND
//2 VCC
//3 CE   - GPIO4
//4 CSN  - GPIO16
//5 SCK  - GPIO14
//6 MOSI - GPIO13
//7 MISO - GPIO12
//8 IRQ  - NC

const uint64_t pipe = 0xC0C0C0C049;

#define RF_MAX_SAMPLE_COUNT 8
typedef struct{
  uint16_t lost_packet_count; //число не принятых приемником пакетов (по которым исчерпано максимальное число попыток передачи и получения ACK)
  uint8_t heart_beat; //счетчик передач для контроля качества канала
  uint8_t status; //статус после получения прерывания завершения предыдущей посылки (0x2X - пакет успешно передан и получен ACK, 0x1X - максимальное число попыток передачи исчерпано, ACK не получен)
  uint8_t clid; //номер передатчика
 //первый байт на каждые 9 - битовая маска знаков значений температур. каждый бит - знак
 //(1 - температура отрицательная, 0 - положительная),
 //номер значения в группе последующих 8 байт массива buf равен номеру соответствующего бита
  uint8_t buf[RF_MAX_SAMPLE_COUNT + (RF_MAX_SAMPLE_COUNT >> 3) + ((RF_MAX_SAMPLE_COUNT % 8) != 0)]; //максимум 27 байт = 24 значения по 8 бит плюс 3 три байта знаковых
} tpacket;

tpacket packet;


const char* ssid = "WatchOut!";
const char* password = "djctvmgjgeufq";

//const char* ssid = "MegaFon_MR100-1";
//const char* password = "95955870";

//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char * mqtt_server = "m10.cloudmqtt.com";
const char * mqtt_user = "quefkobd";
const char * mqtt_password = "524rVRkbylHl";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_mqtt(PubSubClient& cl) {
  cl.setServer(mqtt_server, 16425);
  cl.setCallback(mqtt_callback);  
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("start", "{\"module\":\"bridge\", \"status\":\"connected\"}");
      // ... and resubscribe
      client.subscribe("michael/fazenda/#");
      digitalWrite(BUILTIN_LED, LOW);
    } else {
      digitalWrite(BUILTIN_LED, HIGH);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

RF24 radio(4,16);

void setup_radio(RF24& rad)
{
  rad.begin();
  if (rad.isPVariant()) Serial.println("nRF24L01+ or compatible detected");
  rad.setRetries(0x0F, 0x0F);
  rad.enableDynamicPayloads();  
  rad.setChannel(0x6E);
  if (!rad.setDataRate(RF24_250KBPS)) Serial.println("Failed to set data rate");  // RF24_1MBPS или RF24_2MBPS
  //radio.setDataRate(RF24_250KBPS);
  rad.setCRCLength(RF24_CRC_16);
  rad.setPALevel(RF24_PA_HIGH);
  rad.openReadingPipe(1, /*(uint64_t *)RF_TX_ADDR_VAL*/ pipe);    // открыть канал на приём
  rad.startListening();                                         // приём
  Serial.println("Radio listener started");

}

void setup() {
  
  Serial.begin(9600);

  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);   

  setup_wifi();

  setup_mqtt(client);
  reconnect();

  setup_radio(radio);

}

const char * topic = "sensorvalue"; //"/root/t";

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


    if(radio.available())
    {
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
      String mqstring = String("{\"clientid\":\"root_house_firstfloor\", \"display\":\"testing temperature sensor\", \"value\":\"") + String((packet.buf[0])?'-':' ') + String(t) + String("\", \"time\":\"") + String(packet.heart_beat) + String("\"}");
      
      client.publish(topic, mqstring.c_str());
      
      Serial.println(t);
      
      Serial.print("Buffer: "); 
      for (int i = 0; i < sizeof(packet.buf); i++)
      {
        String str = String(packet.buf[i], HEX) + " ";
        Serial.print(str);
      }
            
      Serial.println();
      Serial.print("Publish: ");
      Serial.println(mqstring);
      Serial.println();
      //delay(1000);
    }  

}
