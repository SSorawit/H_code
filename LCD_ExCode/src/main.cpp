#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Hello World");
}
