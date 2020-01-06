#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

//nRF24L01 - ESP8266 12
//1 GRND
//2 VCC
//3 CE   - GPIO4
//4 CSN  - GPIO16
//5 SCK  - GPIO14
//6 MOSI - GPIO13
//7 MISO - GPIO12
//8 IRQ  - NC

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

void setup_radio_params(RF24& rad);

void setup_radio_listener(RF24& rad, uint64_t pipe);

void setup_radio_transmitter(RF24& rad, uint64_t pipe);


