#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//Blynk--------------------------
#define BLYNK_TEMPLATE_ID "TMPL6lpPmz-xJ"
#define BLYNK_TEMPLATE_NAME "Mini Project"
#define BLYNK_AUTH_TOKEN "UpqPgsdT12qiN9bgrVEVaoTBnCqycb12"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char ssid[] = "XViii";
char pass[] = "53103310";
//-------------------------------
#define DHTPIN D4     
#define DHTTYPE    DHT11     

DHT_Unified dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,4);
const int Relay = D5;
const int watesensor = A0;
const int IDLE = 0;
const int READ_SENSOR = 1;
const int FILL_WATER = 2;
const int SEND_DATA = 3;

int state;
float temp;
float humi;
float waterlevel;
float lavelwater;


void setup() {
  pinMode(D5, OUTPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  state = IDLE;
}

BLYNK_WRITE(V3)
{   
  int value = param.asInt(); // Get value as integer
  Serial.println(value);
  if(value >= 1){
    digitalWrite(D5,HIGH);
  }else{
    digitalWrite(D5,LOW);
  }
}
BLYNK_WRITE(V4)
{   
  int value = param.asInt(); // Get value as integer
  lavelwater = value;
  Serial.print("Current Level Water : ");
  Serial.println(lavelwater);
}

void loop() {
Blynk.run();
sensors_event_t event;
  if(state == IDLE){
    state = READ_SENSOR;
  }
  if(state == READ_SENSOR){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("READ_SENSOR");
    delay(1000);
    dht.temperature().getEvent(&event);
    temp = event.temperature;
    dht.humidity().getEvent(&event);
    humi = event.relative_humidity;
    waterlevel = analogRead(watesensor);
    if(waterlevel > lavelwater){
      state = SEND_DATA;
    }
    else if(waterlevel < lavelwater){
      state = FILL_WATER;
    }
  }
  if(state == FILL_WATER){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FILL_WATER");
    delay(1000);
    digitalWrite(D5, LOW);
    Serial.println("pumpON");
    delay(5000);
    digitalWrite(D5, HIGH);
    Serial.println("pumpOFF");
    delay(5000);
    state = SEND_DATA;

  }
  if(state == SEND_DATA){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SEND_DATA");
    delay(1000);
    lcd.clear();
    Serial.print(F("Temperature : "));
    Serial.print(temp);
    Serial.println(F("°C"));
    lcd.setCursor(0,0);
    lcd.print("Temp : ");
    lcd.print(temp);
    lcd.print(" C");
    Blynk.virtualWrite(V1, temp);
    Serial.print(F("Humidity : "));
    Serial.print(humi);
    Serial.println(F("%"));
    lcd.setCursor(0,1);
    lcd.print("Humi : ");
    lcd.print(humi);
    lcd.print(" %");
    Blynk.virtualWrite(V2, humi);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WaterLv : ");
    lcd.print(waterlevel);
    Serial.print("WaterLv : ");
    Serial.println(waterlevel);
    Blynk.virtualWrite(V0, waterlevel);
    state = IDLE;
    delay(2000);
  }
}