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

const int IDLE = 0;
const int READ_SENSOR = 1;
const int FILL_WATER = 2;
const int SEND_DATA = 3;
int state;
float temp;
float humi;
void setup() {
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
    dht.temperature().getEvent(&event);
    temp = event.temperature;
    dht.humidity().getEvent(&event);
    humi = event.relative_humidity;
    state = SEND_DATA;
  }
  if(state == SEND_DATA){
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
    state = IDLE;
    delay(1000);
  }
}
