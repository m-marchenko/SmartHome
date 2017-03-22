/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/
#include <string.h>
#include <stdlib.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define GPIO2 2
#define BUILTIN_LED GPIO2

#define DS18B20_PIN 2  // DS18B20 pin
#define TEMPERATURE_PRECISION 3
#define DS18B20_ID  "TM0002"

//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

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

#define VALVE_STATE_UNKNOWN  0
#define VALVE_STATE_OPENED   1
#define VALVE_STATE_CLOSED   2

class CValve {
  private:
    byte m_relayPin;
    byte m_statePin;
    
 public:
   CValve(byte relaypin, byte statepin) {
     m_relayPin = relaypin;
     m_statePin = statepin;
     pinMode(m_relayPin, INPUT);
     pinMode(m_statePin, INPUT);          
   }
  
  void Open() {
    pinMode(m_relayPin, OUTPUT);
    digitalWrite(m_relayPin, LOW);    
  }

  void Close() {
    pinMode(m_relayPin, OUTPUT);
    digitalWrite(m_relayPin, HIGH);    
  }

  int getState() {
    int state = digitalRead(m_statePin);  

    Serial.print("pin:"); Serial.print(m_statePin); Serial.print(" state:"); Serial.print(state);
    Serial.println();

    if (state == HIGH)
      return VALVE_STATE_OPENED;
    else
      return VALVE_STATE_CLOSED;

  }
  
  String toJsonString() {
    String result = "{\"state\":" + String(getState(), DEC) + "}";
    
    return result;
  }
  
};

class CLevelSensor{

  public:
    CLevelSensor(){
      
    }

    // возвращает уровень в процентах
    int getLevelValue(){
      return 60;
    }
};

#define VALVE_PIN_TOP     5
#define VALVE_STATE_PIN_TOP 12
#define VALVE_PIN_BOTTOM  14
#define VALVE_STATE_PIN_BOTTOM 13

#define ISBLOCKED 0
#define ISFILLING 1
#define ISEMPTIED 2

class CBarrel {
  private:
    CValve * m_bottomValve;
    CValve * m_topValve;
    CLevelSensor m_level;
    String m_stateSensorId;
    
  public:
    CBarrel(String stateSensorId){
      m_stateSensorId = stateSensorId;
      m_bottomValve = new CValve(VALVE_PIN_BOTTOM, VALVE_STATE_PIN_BOTTOM);
      m_topValve = new CValve(VALVE_PIN_TOP, VALVE_STATE_PIN_TOP);
    }
    
    void Fill(){
      m_bottomValve->Close();
      m_topValve->Open();    
      Serial.println("Barrel is being filled");
    }

    void Empty(){
      m_bottomValve->Open();
      m_topValve->Close();            
      Serial.println("Barrel is being unloaded");
    }

    void Block(){
      m_bottomValve->Close();
      m_topValve->Close();            
      Serial.println("Barrel valves are closed");
    }

    int getState()
    {
      if (m_bottomValve->getState() == VALVE_STATE_OPENED)
        return ISEMPTIED;
      else
        if (m_topValve->getState() == VALVE_STATE_OPENED)
          return ISFILLING;
        
      return ISBLOCKED;
    }

    String toJsonString() {
      int state = getState();
      String stateStr;
      switch (state)
      {
        case ISEMPTIED:
          stateStr = "сливается";
          break;
        case ISFILLING:
          stateStr = "наполняется";
          break;
        case ISBLOCKED:
          stateStr = "перекрыта";
          break;
        
      }
            
      String result = "{\"sensorId\":\"" + m_stateSensorId + "\", \"val\":\"" + stateStr + "\" }";      
      return result;
    }

};

class CJsonCommandParser {
  public: 
    char command[16];
    char target[16];
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
      strcpy(target, root["Target"]);
                  
    }
};


// Update these with values suitable for your network.

const char* ssid = "WatchOut!";
const char* password = "djctvmgjgeufq";

//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char * mqtt_server = "m10.cloudmqtt.com";
const char * mqtt_user = "quefkobd";
const char * mqtt_password = "524rVRkbylHl";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

CThermometer *thermometer;
CBarrel *barrel;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 16425);
  client.setCallback(callback);

  thermometer = new CThermometer(DS18B20_PIN, String(DS18B20_ID));
  barrel = new CBarrel("BST0001");

  //sensors.begin();
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  CJsonCommandParser *cmd = new CJsonCommandParser((const char * )payload);

  Serial.printf("%s recieved command '%s'\r\n", cmd->target, cmd->command);
  
  if (strcmp(cmd->target, "barrel1") == 0) {    
    
    Serial.println("Starting processing command");
    
    if (strcmp(cmd->command, "fill") == 0 ) {
        barrel->Fill();
    } else    
    if (strcmp(cmd->command, "empty") == 0){
        barrel->Empty();
    }
    if (strcmp(cmd->command, "block") == 0){
        barrel->Block();
    }    

    delete cmd;

    Serial.println("Finish processing command");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("start", "{\"module\":\"barrel_dev\", \"status\":\"connected\"}");
      // ... and resubscribe
      client.subscribe("michael/fazenda/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
//delay(200);
//return;
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  long now = millis();
  if (now - lastMsg > 60000) {
    /*
    if (isLow)
      digitalWrite(BUILTIN_LED, LOW);
    else
      digitalWrite(BUILTIN_LED, HIGH);

    isLow = !isLow;
    
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    */

    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(thermometer->getTemperature());  
    Serial.printf("Barrel state: %s", barrel->toJsonString().c_str());
    Serial.println();
    
    //const char * payload = (thermometer->toJsonString()).c_str();
    
    HTTPClient http;
    http.begin("http://smart.no-troubles.com/Command/SetSensorValue");    
    //http.begin("http://192.168.10.109/SmartHome/Command/SetSensorValue");    
    http.addHeader("Content-type", "application/json");
    int httpCode = http.POST(thermometer->toJsonString());
    
    if (httpCode != HTTP_CODE_OK) {
      Serial.printf("[HTTP] Request failed, error: %d %s\n", httpCode, http.errorToString(httpCode).c_str());      
    }
    // sending to mqtt
    //client.publish("sensor", (thermometer->toJsonString()).c_str());
    //client.publish("sensorvalue", (barrel->toJsonString()).c_str());

    httpCode = http.POST(barrel->toJsonString());
    if (httpCode != HTTP_CODE_OK) {
      Serial.printf("[HTTP] Request failed, error: %d %s\n", httpCode, http.errorToString(httpCode).c_str());      
    }
    
    lastMsg = now;
  }
}
