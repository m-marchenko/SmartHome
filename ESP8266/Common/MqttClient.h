#include <PubSubClient.h>

void setup_mqtt(PubSubClient& cl, const char * mqtt_server, int port, MQTT_CALLBACK_SIGNATURE );

void reconnect_mqtt(PubSubClient& cl, const char * mqtt_user, const char * mqtt_password);

