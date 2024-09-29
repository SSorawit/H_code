#include <Arduino.h>
// ตัวแปรสำหรับเซนเซอร์วัดความชื้นและ LED
const int soilSensor = A0; // ต่อเซนเซอร์วัดความชื้นกับขา A0

// ตัวแปรสำหรับกำหนดค่าความชื้นที่ต้องการ
const int moistureThreshold = 900; // ค่าความชื้นที่ต้องการ, สามารถปรับเปลี่ยนได้ตามต้องการ

void setup() {
  Serial.begin(115200);          // เริ่มต้นการสื่อสารผ่าน Serial
}

void loop() {
  int soilMoisture = analogRead(soilSensor); // อ่านค่าจากเซนเซอร์วัดความชื้น
  
  Serial.print("Soil Moisture: ");     // แสดงค่าความชื้นในดิน
  Serial.println(soilMoisture);
  
  delay(1000);    // หน่วงเวลา 1 วินาที แล้ววนลูปอีกครั้ง
}