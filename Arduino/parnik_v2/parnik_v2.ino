#include <ArduinoJson.h>

#include <SoftwareSerial1.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <string.h>
#include <stdlib.h>
#include "Wire.h"

// Clock address
#define DS3231_I2C_ADDRESS 0x68

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 7
#define TEMPERATURE_PRECISION 3
//
//// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);
//
//// Pass our oneWire reference to Dallas Temperature. 
//DallasTemperature sensors(&oneWire);
//
//// arrays to hold device addresses
//DeviceAddress insideThermometer, outsideThermometer;


#define MOTOR_A_SWITCH_PIN   10
#define MOTOR_B_SWITCH_PIN   11
#define MOTOR_DIR_FORWARD    12
#define MOTOR_DIR_BACKWARD   13
#define BUTTON_1_PIN         2
#define BUTTON_2_PIN         3

#define RELAY_1_PIN          8
#define RELAY_2_PIN          9

//class Serializable {
//  public:
//    virtual String toJsonString() {};
//};

struct DateTime {
  public:
  byte second;
  byte minute;
  byte hour;
  byte dayOfWeek;
  byte dayOfMonth;
  byte month;
  byte year;
  
  DateTime() {
    year = month = dayOfMonth = dayOfWeek = hour = minute = second = 0;
  }
  
  String toString() {
    
    String result;

    if (dayOfMonth < 10)
      result += "0";
    result += String(dayOfMonth, DEC) + "-";

    if (month < 10) 
      result += "0";
    result += String(month, DEC) + "-";   
    
    result += "20" + String(year, DEC) + " ";

    if (hour < 10) 
      result += "0";
    result += String(hour, DEC) + ":";   
   
    if (minute < 10) 
      result += "0";
    result += String(minute, DEC) + ":";   
    
    if (second < 10) 
      result += "0";
    result += String(second, DEC);   
    
    return result;
  }
};

class CClock {
  private:
  
    // Convert normal decimal numbers to binary coded decimal
    static byte decToBcd(byte val)
    {
      return( (val/10*16) + (val%10) );
    }
    
    // Convert binary coded decimal to normal decimal numbers
    static byte bcdToDec(byte val)
    {
      return( (val/16*10) + (val%16) );
    }
  
  public:
  
    CClock() {
      Wire.begin();
    }
    
    void setDate(DateTime dt) {
        // sets time and date data to DS3231
        Wire.beginTransmission(DS3231_I2C_ADDRESS);
        Wire.write(0);                      // set next input to start at the seconds register
        Wire.write(decToBcd(dt.second));       // set seconds
        Wire.write(decToBcd(dt.minute));       // set minutes
        Wire.write(decToBcd(dt.hour));         // set hours
        Wire.write(decToBcd(dt.dayOfWeek));    // set day of week (1=Sunday, 7=Saturday)
        Wire.write(decToBcd(dt.dayOfMonth));   // set date (1 to 31)
        Wire.write(decToBcd(dt.month));        // set month
        Wire.write(decToBcd(dt.year));         // set year (0 to 99)
        Wire.endTransmission();    
    }
  
  static void getDate(DateTime & dt) {
    Wire.begin();
    
      Wire.beginTransmission(DS3231_I2C_ADDRESS);
      Wire.write(0); // set DS3231 register pointer to 00h
      Wire.endTransmission();
      Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
      // request seven bytes of data from DS3231 starting from register 00h
      dt.second = bcdToDec(Wire.read() & 0x7f);
      dt.minute = bcdToDec(Wire.read());
      dt.hour = bcdToDec(Wire.read() & 0x3f);
      dt.dayOfWeek = bcdToDec(Wire.read());
      dt.dayOfMonth = bcdToDec(Wire.read());
      dt.month = bcdToDec(Wire.read());
      dt.year = bcdToDec(Wire.read());  
  }
  
  static String getDisplayTime(DateTime & dt) {
    //String result = String(dt.dayOfMonth, DEC) + "-" + String(dt.month, DEC) + "-20" + String(dt.year, DEC) + " " + String(dt.hour, DEC) + ":" + String(dt.minute, DEC) + ":" +String(dt.second, DEC);
    String result = dt.toString();
    return result;
  }
    
};

class CThermometer {
  private:
    String info;
    DallasTemperature *sensor;  
    OneWire *oneWire;
    DeviceAddress thermometer;
    String m_sensorId = "TM0002";

  public:
    // .ctor
    CThermometer(byte pin) {
      oneWire = new OneWire(pin);
      
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

    float getTemperature() {
       sensor->requestTemperatures();
       float value = sensor->getTempC(thermometer);
       return value;
    }
    
    String toJsonString() {
      //String result = "{\"temperature\":" + String(getTemperature(), DEC) + ", \"time\":\"" + getCurrentTime() + "\" }";      
      String result = "{\"sensorId\":\"" + m_sensorId + "\", \"val\":\"" + String(getTemperature(), 0) + "\" }";      
      return result;
    }
    
    String getStatusInfo() {
      return info;
    }
};

String getCurrentTime() {
  DateTime dt;
  CClock::getDate(dt);
  String result = CClock::getDisplayTime(dt);
  return result;
}

class CMotor
{
  byte _switchPin, _forwardPin, _backwardPin;
  public:
  CMotor(byte switchPin, byte forwardPin, byte backwardPin) {
    pinMode(switchPin, OUTPUT);
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
    
    _switchPin = switchPin;
    _forwardPin = forwardPin;
    _backwardPin = backwardPin;
        
  }
  
  void runForward()
  {
    digitalWrite(_switchPin, HIGH);  
    digitalWrite(_forwardPin, HIGH);  
    digitalWrite(_backwardPin, LOW);
  }
  

  void runBackward()
  {
    digitalWrite(_switchPin, HIGH);  
    digitalWrite(_forwardPin, LOW);  
    digitalWrite(_backwardPin, HIGH);
  }
  
  void  Stop()
  {
    digitalWrite(_switchPin, LOW);  
    digitalWrite(_forwardPin, LOW);  
    digitalWrite(_backwardPin, LOW);     
  }
  
};

#define DOOR_STATE_UNKNOWN    0
#define DOOR_STATE_CLOSED     1
#define DOOR_STATE_CLOSING    2
#define DOOR_STATE_OPENED     3
#define DOOR_STATE_OPENING    4
  
class CDoor {
  private:
    CMotor * m_motor;
    byte m_state;
  
  public:

  CDoor() {
    m_motor = new CMotor(MOTOR_A_SWITCH_PIN, MOTOR_DIR_FORWARD, MOTOR_DIR_BACKWARD);
    
    pinMode(BUTTON_1_PIN, INPUT);
    pinMode(BUTTON_2_PIN, INPUT);
    
    attachInterrupt(0, Button1Pressed, RISING);
    attachInterrupt(1, Button2Pressed, RISING); 
    
    m_state = DOOR_STATE_UNKNOWN;
  }
  
  ~CDoor() {
    delete m_motor;
  }
  void Open(){
    m_motor->Stop();
    delay(500);
    
    if (DOOR_STATE_OPENED != m_state) {
      m_motor->runForward();    
      m_state = DOOR_STATE_OPENING;
    }

  }
  
  void Close() {
    m_motor->Stop();
    delay(500);
    
    if (DOOR_STATE_CLOSED != m_state) {
      m_motor->runBackward();        
      m_state = DOOR_STATE_CLOSING;
    }
  }
    
  void Stop() {
    m_motor->Stop();
    
    if (DOOR_STATE_CLOSING == m_state) m_state = DOOR_STATE_CLOSED; else
    if (DOOR_STATE_OPENING == m_state) m_state = DOOR_STATE_OPENED;
  }
  
  byte getState() {
    return m_state;
  }
  
  String toJsonString() {
    String result = "{\"state\":" + String(getState(), DEC) + ", \"time\": \"" + getCurrentTime() + "\"}";
    
    return result;
  }
  
 
};

#define VALVE_STATE_UNKNOWN  0
#define VALVE_STATE_OPENED   1
#define VALVE_STATE_CLOSED   2

class CValve {
  private:
    byte m_relayPin;
    byte m_state;
    
 public:
   CValve(byte pin) {
     m_relayPin = pin;
     pinMode(pin, OUTPUT);
     m_state = VALVE_STATE_UNKNOWN;
   }
  
  void Open() {
    digitalWrite(m_relayPin, LOW);
    m_state = VALVE_STATE_OPENED;
  }

  void Close() {
    digitalWrite(m_relayPin, HIGH);
    m_state = VALVE_STATE_CLOSED;
  }
  
  String toJsonString() {
    String result = "{\"state\":" + String(m_state, DEC) + "}";
    
    return result;
  }
  
};

class CGreenhouse {
  private:
    float m_lowLimit = 25;
    float m_highLimit = 30;  
    CThermometer * m_thermometer;
    CDoor * m_door;
    CValve * m_topValve;
    CValve * m_bottomValve;
    
  public:
  
    CGreenhouse() {
      m_thermometer = new CThermometer(ONE_WIRE_BUS);
      m_door = new CDoor();
      m_topValve = new CValve(RELAY_1_PIN);
      m_bottomValve = new CValve(RELAY_2_PIN);
      
      closeBarrel();
    }
    
    CGreenhouse(float lo, float hi) 
      : CGreenhouse()      
    {
      m_lowLimit = lo;
      m_highLimit = hi;
    }
    
    ~CGreenhouse() {
      delete m_thermometer;
      delete m_door;
      delete m_topValve;
      delete m_bottomValve;
    }
    
    CThermometer * getThermometer(){
      return m_thermometer;
    }
    
    CDoor * getDoor() {
      return m_door;
    }
    
    float getLowLimit() {
      return m_lowLimit;
    }
    
    float getHighLimit() {
      return m_highLimit;
    }
    
    void setOpenRange(float lo, float hi){
      m_lowLimit = lo;
      m_highLimit = hi;
    }
    
    void fillBarrel() {
      m_bottomValve->Close();
      m_topValve->Open();
    }

    void emptyBarrel() {
      m_topValve->Close();
      m_bottomValve->Open();
    }
    
    void closeBarrel() {
      m_topValve->Close();
      m_bottomValve->Close();      
    }
    
    String toJsonString() {
      String result = "{ \"thermometer\":" + m_thermometer->toJsonString();
      result += ", \"door\":" + m_door->toJsonString();
      result += ", \"tlow\":" + String(m_lowLimit, DEC);
      result += ", \"thigh\":" + String(m_highLimit, DEC);
      result += ", \"top_valve\":" + m_topValve->toJsonString();
      result += ", \"bottom_valve\":" + m_bottomValve->toJsonString();
      result += "}";
      
      return result;
    }
}; 

class CJsonCommandParser {
  public: 
    char command[16];
    const char * target;
    bool isSuccess = true;
    
    String params [5];
    int param_num = 0;
    
    StaticJsonBuffer<200> jsonBuffer;

    CJsonCommandParser(const char * cmdLine) {
      JsonObject& root = jsonBuffer.parseObject(cmdLine);
  
      if (!root.success()) {
        isSuccess = false;
        return;    
      }      

      strncpy(command, root["Command"],16);
      target = root["Target"];
                  
    }
};

//CThermometer *thermo;
CGreenhouse * greenhouse;

SoftwareSerial softSerial(10, 11); // RX, TX


void Button1Pressed() {

    //Serial.println("button 1 pressed");    
    
    greenhouse->getDoor()->Stop();
}

void Button2Pressed() {

    //Serial.println("button 2 pressed");    
    
    greenhouse->getDoor()->Stop();
}


//int index(25000);

void setup() {
  // put your setup code here, to run once:
  
  // start serial port at 9600 bps:
  Serial.begin(9600);
  //softSerial.begin(9600);
 
  greenhouse = new CGreenhouse();
  
  Serial.println(greenhouse->getThermometer()->getStatusInfo());

  
}
const long interval = 60000;
unsigned long time_last = 0;

void loop() {

    unsigned long time_now = millis();

    if (time_now - time_last >= interval) {
        
        time_last = time_now;
        
        float current_T = greenhouse->getThermometer()->getTemperature();
    
        if (current_T < greenhouse->getLowLimit()) {
          if (greenhouse->getDoor()->getState() != DOOR_STATE_CLOSED)
            greenhouse->getDoor()->Close();
        }
      
        if (current_T > greenhouse->getHighLimit()) {
          if (greenhouse->getDoor()->getState() != DOOR_STATE_OPENED)
            greenhouse->getDoor()->Open();
        }
        
        //if (current_T < greenhouse->getHighLimit() &&  current_T > greenhouse->getLowLimit())
        //   greenhouse->getDoor()->Stop();
    
        Serial.write((greenhouse->getThermometer()->toJsonString() + "\r").c_str());
    }
}


// Available commands:
// set_range lo, hi  - set range of temperature 
// opendoor
// closedoor
// stop_door
// fill
// empty
// block
String _command;
void serialEvent() {
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    _command += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    //Serial.print(inChar);
    if (inChar == '\n') {      
      Serial.println("Input: " + _command);
      
      CJsonCommandParser *cmd = new CJsonCommandParser(_command.c_str());            
      _command = "";
      
      if (strcmp(cmd->command, "set_range") == 0){
        float lo = atof((cmd->params[0]).c_str());
        float hi = atof((cmd->params[1]).c_str());
        
        greenhouse->setOpenRange(lo, hi);        
      } else
      if (strcmp(cmd->command, "opendoor") == 0){
        greenhouse->getDoor()->Open();
      } else
      if (strcmp(cmd->command, "closedoor") == 0){
        greenhouse->getDoor()->Close();
      } else
      if (strcmp(cmd->command, "stop_door") == 0){
        greenhouse->getDoor()->Stop();
      } else
      if (strcmp(cmd->command, "fill") == 0){
        greenhouse->fillBarrel();
      } else
      if (strcmp(cmd->command, "empty") == 0){
        greenhouse->emptyBarrel();
      } else
      if (strcmp(cmd->command, "block") == 0){
        greenhouse->closeBarrel();
      } else
      {
        Serial.println("Command " + String(cmd->command) + " is undefined");
      }
      
      
      Serial.println("Command " + String(cmd->command) + " executed");
      
//      ExecuteCommand(cmd);
      
      
      
      delete cmd;

    }
  }
}



