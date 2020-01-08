#include "MqttClient.h"

void setup_mqtt(PubSubClient& cl, const char * mqtt_server, int port, MQTT_CALLBACK_SIGNATURE ) {
  cl.setServer(mqtt_server, port);
  cl.setCallback(callback);  
}


void reconnect_mqtt(PubSubClient& cl, const char * mqtt_user, const char * mqtt_password) {
  // Loop until we're reconnected
  while (!cl.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (cl.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      cl.publish("start", "{\"module\":\"bridge\", \"status\":\"connected\"}");
      // ... and resubscribe
      cl.subscribe("michael/fazenda/#");
      digitalWrite(BUILTIN_LED, LOW);
    } else {
      digitalWrite(BUILTIN_LED, HIGH);
      Serial.print("failed, rc=");
      Serial.print(cl.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

