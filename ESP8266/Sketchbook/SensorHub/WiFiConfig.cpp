#include <string.h>
#include <stdlib.h>
#include <ESP8266WiFi.h>


//const char* ssid = "WatchOut!";
//const char* password = "djctvmgjgeufq";

//const char* ssid = "MegaFon_MR100-1";
//const char* password = "95955870";

void setup_wifi(const char* ssid, const char* password) {

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
