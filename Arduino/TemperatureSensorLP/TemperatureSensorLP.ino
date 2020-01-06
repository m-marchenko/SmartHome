#include <LiquidCrystal_PCF8574.h>

#include <string.h>
#include <stdlib.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20_PIN 7  // DS18B20 pin
#define DS18B20_ID  "TM0005"
#define TEMPERATURE_PRECISION 3
#define INTERVAL 10000
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

//CThermometer *thermometer;
long _lastMsg = 0;

LiquidCrystal_PCF8574 lcd(0x27);

void power_on() {
  //pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
}

void power_off() {
  //pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
}

uint8_t grad[8] = {
  B01110,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
};

uint8_t cross[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
//uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting...");

  pinMode(POWER_PIN, OUTPUT);
  power_off();
  //thermometer = new CThermometer(DS18B20_PIN, String(DS18B20_ID));

  //Serial.println(thermometer->getStatusInfo());
  lcd.createChar(1, cross);
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  lcd.print("Starting...");
}

void loop() {
  // put your main code here, to run repeatedly:

  long now = millis();
  if (now - _lastMsg > INTERVAL ) {

    Serial.println(millis());
    power_on();
    CThermometer *thermometer = new CThermometer(DS18B20_PIN, String(DS18B20_ID));
    float t = thermometer->getTemperature();
    power_off();
    Serial.println(millis());
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(t);  

    lcd.home(); 
    lcd.clear();
    lcd.print("T");//lcd.write((uint8_t)1); 
    lcd.print(" = ");
    lcd.print(t);

    delete thermometer;
    
    _lastMsg = now;
  }
}
