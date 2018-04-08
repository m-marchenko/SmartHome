#include <string.h>
#include <stdlib.h>
#include "ESP-nRFConfig.h"

void setup_radio(RF24& rad, uint64_t pipe)
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
