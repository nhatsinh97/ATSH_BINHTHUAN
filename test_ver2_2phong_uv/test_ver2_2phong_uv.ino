/*************************************************************************
   VERSION 2022-V0.1
   05/02/2022
   PHIÊN BẢN CHO MAIN ESP32 CÓ 2 KÊNH DAC 0-10V
   6 RELAY
 ************************************************************************/
#include <ArduinoJson.h>
#include "RTClib.h"
#include "EEPROM.h"
#include <Wire.h>
#include <WiFi.h>
#include <Modbus.h>
#include <ModbusMaster.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MCP4725.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //3F
RTC_DS3231 rtc;
//#include <esp_task_wdt.h>
//#define WDT_TIMEOUT 3
char P1[10] = "Backup", P2[10] = "Backup";
unsigned long previousMillis = 0;
const long interval = 1000; // giá trị delay (milliseconds)
int dem, i;
int gio, phut, giay;
Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;
String sendRaspberry = "";
int status_uv1, status_uv2;
int gio1, phut1, giay1;
int gio2, phut2, giay2;
long cb_cua1 = 0, cb_cua2 = 0;

int RL_1 = 15, RL_2 = 13, RL_3 = 12, RL_4 = 32, RL_5 = 33, RL_6 = 25;
int BT_1 = 36, BT_2 = 39, BT_cua1 = 34, BT_cua2 = 4;

#define SerialComputer  Serial
#define SerialModbus    Serial2
void setup()
{
//  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
//  esp_task_wdt_add(NULL); //add current thread to WDT watch
  SerialComputer.begin(9600);
  SerialModbus.begin(9600);
  Wire.begin();
  rtc.begin();
  lcd.init();
  lcd.backlight();
  pinMode(BT_1, INPUT_PULLUP);
  pinMode(BT_2, INPUT_PULLUP);
  pinMode(BT_cua1, INPUT_PULLUP);
  pinMode(BT_cua2, INPUT_PULLUP);
  pinMode(RL_1, OUTPUT);
  pinMode(RL_2, OUTPUT);
  pinMode(RL_3, OUTPUT);
  pinMode(RL_4, OUTPUT);
  pinMode(RL_5, OUTPUT);
  pinMode(RL_6, OUTPUT);

  digitalWrite(RL_1, LOW);
  digitalWrite(RL_2, LOW);
  digitalWrite(RL_3, LOW);
  digitalWrite(RL_4, LOW);
  digitalWrite(RL_5, LOW);
  digitalWrite(RL_6, LOW);
  
  
  int address = 0;
  sendRaspberry = EEPROM.readString(address);
  dac1.begin(0x60);
  dac2.begin(0x61);
  dac1.setVoltage(0, false);
  dac2.setVoltage(0, false);

  lcd.print("  PHONG UV TU DONG"); lcd.setCursor(0 , 2); lcd.print("  Ver: 2.0"); delay(1000); lcd.clear();
  StaticJsonDocument<500> doc;
  deserializeJson(doc, sendRaspberry);
  gio1  = doc["phonguv1"]["gio1"];
  phut1 = doc["phonguv1"]["phut1"];
  giay1 = doc["phonguv1"]["giay1"];
  status_uv1 = doc["phonguv1"]["status_uv1"];
  gio2  = doc["phonguv2"]["gio2"];
  phut2 = doc["phonguv2"]["phut2"];
  giay2 = doc["phonguv2"]["giay2"];
  status_uv2 = doc["phonguv2"]["status_uv2"];
  //************************ NÚT NHẤN 1 ******************************//
  if (status_uv1 > 0) {
    strcpy(P1, " ON   ");
    if (status_uv1 == 1){
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    oz1(); delay(1000);
    }
    else{
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    uv1();
    }
  }
  if (status_uv2 > 0) {
    strcpy(P2, " ON   ");
    if (status_uv2 == 1){
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    oz2(); delay(1000);
    }
    else{
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    uv2();
    }
  }
  if (status_uv1 == 0){
    strcpy(P1, " OFF  ");
  }
  if (status_uv2 == 0){
    strcpy(P2, " OFF  ");
  }
}

void loop()
{ 
//  esp_task_wdt_reset();
  unsigned long currentMillis = millis();
  //************************ NÚT NHẤN 1 ******************************//
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0)) {
    delay(100);
  }
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0)) {
    delay(100);
  }
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0)) {
    delay(100);
  }
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0)) {
    status_uv1 = 1;
    strcpy(P1, " ON   ");
    DateTime now = rtc.now();
    DateTime future1 (now + TimeSpan(0, 1, 0, 0));
    gio1 = future1.hour(), DEC;
    phut1 = future1.minute(), DEC;
    giay1 = future1.second(), DEC;
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    oz1(); delay(1000);
  }
  //************************ NÚT NHẤN 2 ******************************//
  if ((status_uv2 == 0) && (digitalRead (BT_2) == 0)) {
    delay(100);
  }
  if ((status_uv2 == 0) && (digitalRead (BT_2) == 0)) {
    delay(100);
  }
  if ((status_uv2 == 0) && (digitalRead (BT_2) == 0)) {
    delay(100);
  }
  if ((status_uv2 == 0) && (digitalRead (BT_2) == 0)) {
    status_uv2 = 1;
    strcpy(P2, " ON   ");
    DateTime now = rtc.now();
    DateTime future2 (now + TimeSpan(0, 1, 0, 0));
    gio2 = future2.hour(), DEC;
    phut2 = future2.minute(), DEC;
    giay2 = future2.second(), DEC;
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    oz2(); delay(1000);
  }
  //************************ END NÚT NHẤN ******************************//

  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV1 ****//
  if (digitalRead (BT_cua1) == 0) {
    cb_cua1 = 1;
    for (i = 1; i <= 100; i++) {
      lcd.clear();
      lcd.setCursor(0 , 0); lcd.print("Chu y: Cua so 1");
      lcd.setCursor(0 , 1); lcd.print("       dang mo !");
      lcd.setCursor(9 , 2); lcd.print(i);
      if (i == 1) {
        datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
      }
      if (digitalRead (BT_cua1) == 1) {
        cb_cua1 = 0;
        datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
        lcd.clear();
        break;
      }
      delay(1000);
    }
  }
  //**** END NÚT NHẤN PHÒNG UV1 ****//
  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV2 ****//
  if (digitalRead (BT_cua2) == 0) {
    for (i = 1; i <= 100; i++) {
      lcd.clear();
      lcd.setCursor(0 , 0); lcd.print("Chu y: Cua so 2");
      lcd.setCursor(0 , 1); lcd.print("       dang mo !");
      lcd.setCursor(9 , 2); lcd.print(i);
      cb_cua2 = 1;
      if (i == 1) {
        datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
      }
      if (digitalRead (BT_cua2) == 1) {
        cb_cua2 = 0;
        datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
        lcd.clear();
        break;
      }
      delay(1000);
    }
  }
  //**** END NÚT NHẤN PHÒNG UV2 ****//
  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV1 ****//
  if (digitalRead (BT_cua2) == 0) {

  }
  //**** END NÚT NHẤN PHÒNG UV1 ****//
  //************************ START KIỂM TRA THỜI GIAN CHẠY ******************************//
  if ( gio >= gio1 && phut >= phut1 && giay >= giay1 && status_uv1 == 1 ) {
    DateTime now = rtc.now();
    DateTime future1 (now + TimeSpan(0, 0, 30, 0));
    gio1 = future1.hour(), DEC;
    phut1 = future1.minute(), DEC;
    giay1 = future1.second(), DEC;
    uv1(); status_uv1 = 2; delay(1000);
  }
  if ( gio >= gio2 && phut >= phut2 && giay >= giay2 && status_uv2 == 1 ) {
    DateTime now = rtc.now();
    DateTime future2 (now + TimeSpan(0, 0, 30, 0));
    gio2 = future2.hour(), DEC;
    phut2 = future2.minute(), DEC;
    giay2 = future2.second(), DEC;
    uv2(); status_uv2 = 2;
  }
  if ( gio >= gio1 && phut >= phut1 && giay >= giay1 && status_uv1 == 2 ) {
    off1();
  }
  if ( gio >= gio2 && phut >= phut2 && giay >= giay2 && status_uv2 == 2 ) {
    off2();
  }
  //************************ END KIỂM TRA THỜI GIAN CHẠY ******************************//
  if (dem == 60) { dem = 0;
    datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
    //**** START Nếu phòng UV được bật thì lưu dữ liệu vào eeprom ****//
    int address = 0;
    EEPROM.writeString(address, sendRaspberry);
    EEPROM.commit();
    //**** END Nếu phòng UV được bật thì lưu dữ liệu vào eeprom ****//
  }
  //***** lấy giá trị hiện tại trừ đi giá trị trước đó và so sánh với khoản thời gian delay ******//
  if (currentMillis - previousMillis >= interval) {
    // nếu giá trị đó bằng hoặc vượt qua
    // lưu giá trị hiện tại thành giá trị lần cuối cập nhật
    previousMillis = currentMillis;
    lcdout(); dem++;
  }
  //nếu giá trị chưa vượt qua thì không làm gì cả
  
}
//********************************* END LOOP **************************************************//
//**** START PHÒNG UV 1 ****//
void uv1() {
  digitalWrite(RL_1, HIGH);
  digitalWrite(RL_2, LOW);
  digitalWrite(RL_3, HIGH);
}
void oz1() {
  digitalWrite(RL_1, LOW);
  digitalWrite(RL_2, HIGH);
  digitalWrite(RL_3, HIGH);
}
void off1() {
  gio1 = 0, phut1 = 0, giay1 = 0, status_uv1 = 0;
  strcpy(P1, " OFF  "); lcd.clear();
  digitalWrite(RL_1, LOW);
  digitalWrite(RL_2, LOW);
  digitalWrite(RL_3, LOW);
  datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
}
//**** END PHÒNG UV1 ****//
//**** START PHÒNG UV2 ****//
void uv2() {
  digitalWrite(RL_4, HIGH);
  digitalWrite(RL_5, LOW);
  digitalWrite(RL_6, HIGH);
}
void oz2() {
  digitalWrite(RL_4, LOW);
  digitalWrite(RL_5, HIGH);
  digitalWrite(RL_6, HIGH);
}
void off2() {
  gio2 = 0, phut2 = 0, giay2 = 0, status_uv2 = 0;
  strcpy(P2, " OFF  "); lcd.clear();
  digitalWrite(RL_4, LOW);
  digitalWrite(RL_5, LOW);
  digitalWrite(RL_6, LOW);
  datajson(String(status_uv1), String(status_uv2), String(gio1), String(phut1), String (giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
}
//**** END PHÒNG UV 2 ****//
void lcdout() {
//  Serial.println(gio1);Serial.println(phut1);Serial.println(giay1);Serial.println(status_uv1);
//  Serial.println(gio2);Serial.println(phut2);Serial.println(giay2);Serial.println(status_uv2);
  DateTime now = rtc.now();
  gio = now.hour(), DEC; phut = now.minute(), DEC; giay = now.second(), DEC;
  lcd.setCursor(1, 0); lcd.print(now.year(), DEC);
  lcd.print('/'); lcd.print(now.month(), DEC);
  lcd.print('/'); lcd.print(now.day(), DEC);
  lcd.print(' '); lcd.print(now.hour(), DEC);
  lcd.print(':'); lcd.print(now.minute(), DEC);
  lcd.print(':'); lcd.print(now.second(), DEC);
  lcd.print(' ');
  lcd.setCursor(0 , 1); lcd.print("P1:");
  lcd.setCursor(10, 1); lcd.print("P2:");
  lcd.setCursor(3 , 1); lcd.print(P1);
  lcd.setCursor(13, 1); lcd.print(P2);
  lcd.setCursor(0 , 2); lcd.print(gio1);
  lcd.print(":"); lcd.print(phut1); lcd.print(":");
  lcd.print(giay1); lcd.print(" ");
  lcd.setCursor(10, 2); lcd.print(gio2);
  lcd.print(":"); lcd.print(phut2); lcd.print(":");
  lcd.print(giay2); lcd.print(" ");
//  esp_task_wdt_reset();
}
//**** SEND DATA JSON TO RASPBERRY ****//
void datajson(String status_uv1, String status_uv2, String gio1, String phut1, String giay1, String gio2, String phut2, String giay2, String cb_cua1, String cb_cua2) {
  sendRaspberry = "";
  sendRaspberry = "{ \"phonguv1\": { \"status_uv1\":\"" + String(status_uv1) + "\"," +
                  "\"cb_cua1\": \"" + String(cb_cua1) + "\"," +
                  "\"gio1\":  \"" + String(gio1)  + "\"," +
                  "\"phut1\": \"" + String(phut1) + "\"," +
                  "\"giay1\": \"" + String(giay1) + "\"}" +
                  "," +
                  "\"phonguv2\": { \"status_uv2\":\"" + String(status_uv2) + "\"," +
                  "\"cb_cua2\": \"" + String(cb_cua2) + "\"," +
                  "\"gio2\":  \"" + String(gio2)  + "\"," +
                  "\"phut2\": \"" + String(phut2) + "\"," +
                  "\"giay2\": \"" + String(giay2) + "\"}" + "}";

  StaticJsonDocument<500> doc;
  deserializeJson(doc, sendRaspberry);
  serializeJsonPretty(doc, Serial);
}
//**** END SEND DATA JSON TO RASPBERRY ****//
