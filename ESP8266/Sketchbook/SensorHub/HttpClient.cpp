#include <string.h>
#include <stdlib.h>
#include <ESP8266HTTPClient.h>

void post_message(const char *url, String message) {

    HTTPClient http;
    http.begin(url);
	  http.addHeader("Content-type", "application/json");
    int httpCode = http.POST(message);
    
    if (httpCode != HTTP_CODE_OK) {
      Serial.printf("[HTTP] Request failed, error: %d %s\n", httpCode, http.errorToString(httpCode).c_str());      
    }
  
 }
