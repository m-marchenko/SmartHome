#include <string.h>
#include <stdlib.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "ESP-nRFConfig.h"

#include <SPI.h>

#define DS18B20_PIN 7  // DS18B20 pin
#define TEMPERATURE_PRECISION 3
#define DS18B20_ID  "TM0005"
#define TRANSMITTER_CLID  5
#define INTERVAL 60000
#define POWER_PIN 5

class CThermometer {
  private:
    String info;
    DallasTemperature *sensor;  
    OneWire *oneWire;
    DeviceAddress thermometer;
    String m_sensorId;

  public:
    // .ctor
    CThermometer(byte pin, String id) {      
      oneWire = new OneWire(pin);      
      m_sensorId = id;
      
      sensor = new DallasTemperature(oneWire);    
      sensor->begin();
      
      if(sensor->getDeviceCount() == 0)
      {
        info = "No temperature sensors found";
        return;
      }
      
      if (!sensor->getAddress(thermometer, 0))
      {
        info = "Unable to get sensor address";
        return;
      }
      
      sensor->setResolution(thermometer, TEMPERATURE_PRECISION);
      
      info = "Thermometer successfully initialized";
    }

    ~CThermometer() {
      if (NULL != oneWire)
        delete oneWire;

      if (NULL != sensor)
        delete sensor;
    }

    float getTemperature() {
       sensor->requestTemperatures();
       //float value = sensor->getTempC(thermometer);
       float value = sensor->getTempCByIndex(0);
       return value;
    }
    
    String toJsonString() {      
      String result = "{\"sensorId\":\"" + m_sensorId + "\", \"val\":\"" + String(getTemperature(), 0) + "\" }";      
      return result;
    }
    
    String getStatusInfo() {
      return info;
    }

};

void power_on() {
  //pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
}

void power_off() {
  //pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
}


tpacket _packet;
uint16_t _lost = 0;
uint8_t _tick = 0;
uint8_t _status = 0x00;

const uint64_t _pipe = 0xC0C0C0C049;
RF24 radio(9,10);


//CThermometer *thermometer;
long _lastMsg = 0;



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Starting...");
/*
  SPI.setHwCs(true);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);   
*/

  _packet.lost_packet_count = _lost;
  _packet.heart_beat = _tick;
  _packet.status = _status;
  _packet.clid = TRANSMITTER_CLID;

  setup_radio_transmitter(radio, _pipe);

  pinMode(POWER_PIN, OUTPUT);
  power_off();
  
  //thermometer = new CThermometer(DS18B20_PIN, String(DS18B20_ID));

  //Serial.println(thermometer->getStatusInfo());
}

void loop() {
  // put your main code here, to run repeatedly:

  long now = millis();
  if (now - _lastMsg > INTERVAL || _lastMsg == 0) {

    power_on();
    CThermometer *thermometer = new CThermometer(DS18B20_PIN, String(DS18B20_ID));
    float t = thermometer->getTemperature();
    power_off();
    
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(t);  

    _packet.lost_packet_count = _lost;
    _packet.heart_beat = _tick;
    _packet.status = _status;    
    _packet.buf[0] = (t < 0) ? 1 : 0;
    _packet.buf[1] = int(abs(t)*2);

    if (radio.write(&_packet, sizeof(_packet))) {
      _status = 0x2E;
    }
    else {
      _status = 0x1E;
      _lost++;
    }

    Serial.print("Message is sent. Status = "); 
    Serial.print(_status); 
    Serial.print(" Ticks = "); 
    Serial.println(_tick);
    
    _tick++;

    delete thermometer;
    
    _lastMsg = now;
  }
}
