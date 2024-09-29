#include <Arduino.h>

int Relay = D2; // กำหนดขาใช้งาน
void setup()
{
Serial.begin(115200);
pinMode(D2, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา 2 เป็น OUTPUT
}
void loop()
{
digitalWrite(D2, LOW); // ส่งให้ไฟติด
Serial.println("on");
delay(2000); // ดีเลย์ 1000ms
digitalWrite(D2, HIGH); // ส่งให้ไฟดับ
Serial.println("off");
delay(2000); // ดีเลย์ 1000ms
}