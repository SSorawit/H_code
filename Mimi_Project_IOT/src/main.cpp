#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

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
int waterlevel;
void setup() {
  pinMode(D5, OUTPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  dht.begin();
  state = IDLE;
}

void loop() {
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
    if(waterlevel > 200){
      state = SEND_DATA;
    }
    else if(waterlevel < 200){
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
    Serial.print(F("Humidity : "));
    Serial.print(humi);
    Serial.println(F("%"));
    lcd.setCursor(0,1);
    lcd.print("Humi : ");
    lcd.print(humi);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WaterLv : ");
    lcd.print(waterlevel);
    Serial.println(waterlevel);
    state = IDLE;
    delay(2000);
  }
}
