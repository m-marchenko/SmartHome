#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <SPI.h>

RF24 radio(9,10);
const uint64_t pipe = 0xC0C0C0C049;
//const uint32_t pipe = 0x49C0C0C0C0;

const unsigned char  RF_TX_ADDR_VAL[5] = {0x49, 0xC0, 0xC0, 0xC0, 0xC0}; 

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
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_radio(radio);
}

void loop() {
  // put your main code here, to run repeatedly:

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
                 
      Serial.println(t);
      
      Serial.print("Buffer: "); 
      for (int i = 0; i < sizeof(packet.buf); i++)
      {
        String str = String(packet.buf[i], HEX) + " ";
        Serial.print(str);
      }
            
      Serial.println();
      Serial.println();
      //delay(1000);
    }  
}
