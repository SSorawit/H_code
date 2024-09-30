#include <Arduino.h>

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

// MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io";   // EMQX broker endpoint
const char *mqtt_topic = "demo/esp8266";      // MQTT topic
// const char *mqtt_username = "admin";          // MQTT username for authentication
// const char *mqtt_password = "cpe2541";        // MQTT password for authentication
const int mqtt_port = 1883;                   // MQTT port (TCP)

WiFiClient espClient;
PubSubClient mqtt_client(espClient);
String mqttMessage;

void setupWiFiManager();
void connectToMQTTBroker();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void WiFi_setup();

int state;
int LED_ON = 1;
int LED_OFF = 2;
void setup() {

  Serial.begin(115200);
  state = 2;
  pinMode(D2,OUTPUT);
  pinMode(D1,INPUT);
  WiFi_setup();

  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();

}

void loop() {
    mqtt_client.loop();
    if(state == LED_OFF){
        digitalWrite(D2,LOW);
        mqttMessage.toLowerCase();
        if(mqttMessage == "on" || digitalRead(D1) == 1){
            state = LED_ON;
        }
    }
    if(state == LED_ON){
        digitalWrite(D2,HIGH);
        mqttMessage.toLowerCase();
        if(mqttMessage == "off" || digitalRead(D1) == 1){
            state = LED_OFF;
        }
    }
}

void WiFi_setup() {
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    res = wm.autoConnect("ESP8266_test"); // anonymous ap
    // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }

}

void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        // if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        if (mqtt_client.connect(client_id.c_str())) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    mqttMessage = "";
    for (unsigned int i=0;i<length;i++) {
        mqttMessage += (char)payload[i];  // Convert *byte to string
    }
    Serial.println(mqttMessage);
}