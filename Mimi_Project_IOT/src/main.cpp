#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266HTTPClient.h>
//Host & httpsPort----------------
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client; //--> Create a WiFiClientSecure object.
String GAS_ID = "AKfycbwHbjsdQ2WOtUd_cSvrYLMeTPUnXNpG8nBKNMMaRd3BrnzBNK8zOB2M31PH_eeECeyF"; //--> spreadsheet script ID
void sendData(float value,float value2,float value3);
//LineNotify-------------------------------------------------------
#define LINE_TOKEN "8Gqa0YR3crPG7xhcJRtkXm8HdOYSz9f8VHdN2y68dlv"
HTTPClient http;
void sendLineNotification(String message);
void sendLineNotificationValue(String varName, float value);
//Blynk--------------------------
#define BLYNK_TEMPLATE_ID "TMPL6lpPmz-xJ"
#define BLYNK_TEMPLATE_NAME "Mini Project"
#define BLYNK_AUTH_TOKEN "UpqPgsdT12qiN9bgrVEVaoTBnCqycb12"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char ssid[] = "XViii";
char pass[] = "0653514914";
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
float lavelwater = 20;


void setup() {
  pinMode(D5, OUTPUT);
  digitalWrite(D5, HIGH);
  Serial.begin(115200);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  dht.begin();
  // การเชื่อมต่อ WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 0);
    lcd.print("Connecting WiFi");
  }
  Serial.println("WiFi connected");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  state = IDLE;
  client.setInsecure();
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
    Blynk.run();
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
    sendData(temp,humi,waterlevel);
    delay(8000);
    // ส่งการแจ้งเตือน LINE Notify
    sendLineNotificationValue("Temperature", temp);
    delay(1000);
    sendLineNotificationValue("Humidity", humi);
    delay(1000);
    sendLineNotificationValue("Water Level", waterlevel);
    delay(1000);
    state = IDLE;
    delay(2000);
  }
}

//test_script
//https://script.google.com/macros/s/AKfycbwHbjsdQ2WOtUd_cSvrYLMeTPUnXNpG8nBKNMMaRd3BrnzBNK8zOB2M31PH_eeECeyF/exec
//https://script.google.com/macros/s/AKfycbwHbjsdQ2WOtUd_cSvrYLMeTPUnXNpG8nBKNMMaRd3BrnzBNK8zOB2M31PH_eeECeyF/exec?temp=28&humi=65

void sendData(float value, float value2, float value3) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Connect to Google host
  if (!client.connect(host, httpsPort)) {
    lcd.setCursor(0, 0);
    lcd.print("GAS conn failed");
    return;
  }

  // Prepare and send data  
  float string_temp = value; 
  float string_humi = value2;
  float string_waterlevel = value3;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi + "&wlevel=" + string_waterlevel; // Send 3 variables 
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");

  // Wait for response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successful!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  
  Serial.print("reply was: ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
}

void sendLineNotification(String message)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    client.setInsecure(); // สำหรับการเชื่อมต่อ HTTPS แบบไม่ตรวจสอบใบรับรอง
    http.begin(client, "https://notify-api.line.me/api/notify");
 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Bearer " + String(LINE_TOKEN));
 
    String payload = "message=" + message;
    int httpResponseCode = http.POST(payload);
 
    if (httpResponseCode > 0)
    {
      Serial.print("LINE Notify Response Code: ");
      Serial.println(httpResponseCode);
    }
    else
    {
      Serial.print("Error sending LINE Notify: ");
      Serial.println(httpResponseCode);
    }
 
    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
  }
}

void sendLineNotificationValue(String varName, float value)
{
  String strValue = String(value); // แปลงจาก int เป็น String
  {
    sendLineNotification(varName + " = " + strValue);
  }
}