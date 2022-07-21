#include <Wire.h>
#include "Timer.h"
#include <ArduinoJson.h> //v5.13.5
#include <HTTPClient.h>
#include <WiFiMulti.h>         // Thư viện dùng để kết nối WiFi đa cấu hình của ESP8266
#include <Arduino.h>
#include <HTTPUpdate.h>
WiFiMulti wifiMulti;           // Khởi tạo đối tượng wifiMulti từ ESP8266WiFiMulti
void update_FOTA();
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //Thiết lập địa chỉ và loại LCD
uint32_t updateCounter = 0;
uint32_t start_time, time_uv1, time_uv2;
String version = "2.4", key = "37f0d021-9105-4822-8304-fed5b365c89d";
int mVperAmp = 185, Watt = 0, i, statusuv1 = 0, statusuv2 = 0, gui = 0;
double Voltage = 0, VRMS = 0, AmpsRMS = 0;
byte TRIAC1 = 16, TRIAC2 = 17, TRIAC3 = 14, TRIAC4 = 18;
byte BT1 = 25, BT2 = 26, BT3 = 39, BT4 = 36;
byte SW1 = 32, SW2 = 33, SW3 = 34, SW4 = 35, LOA = 23;
long value_uv1 = 0, value_uv2 = 0, bigUV1 = 0, bigUV2 = 0 ;
const int ACS1 = 4, ACS2 = 19;      // pin where the OUT pin from sensor is connected on Arduino
char e[100]       = "Backup ";
char a1[100]       = " ON    ";
char a2[100]       = " ON    ";
char d1[100]       = " OFF   ";
char d2[100]       = " OFF   ";
char startus[100] = " OFF   ";
void setup() {
  Serial.begin (115200);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("  PHONG UV TU DONG"); lcd.setCursor(0 , 2); lcd.print("  Ver: 2.0"); delay(1000); lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Connecting ... ");
  wifiMulti.addAP("baove", "");
  wifiMulti.addAP("TOTOLINK N300RH", "");
  wifiMulti.addAP("GreenFeed", "2020@GREENFEED");
  // Chờ kết nối WiFi được thiết lập. Quét tìm mạng WiFi và kết nối với mạng mạnh nhất.
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print(i);
    i++;
    if (i >= 50) {
      esp_restart();
    }
  }
  lcd.clear(); lcd.setCursor(0, 0);
  lcd.print("Connected to :");
  lcd.setCursor(0, 1); lcd.print(WiFi.SSID());
  lcd.setCursor(2, 2); lcd.print("IP address: ");
  lcd.setCursor(0, 3); lcd.print(WiFi.localIP());
  delay(5000); lcd.clear();
  pinMode(TRIAC1, OUTPUT); pinMode(TRIAC2, OUTPUT); pinMode(TRIAC3, OUTPUT); pinMode(TRIAC4, OUTPUT), pinMode(LOA, OUTPUT);
  pinMode(BT1, INPUT); pinMode(BT2, INPUT); pinMode(BT3, INPUT); pinMode(BT4, INPUT);
  pinMode(SW1, INPUT); pinMode(SW2, INPUT); pinMode(SW3, INPUT); pinMode(SW4, INPUT);
}
void loop() {
  


  /*============== PHÒNG UV1 START ====================== */
  if ((value_uv1 == 0) && (digitalRead (BT4) == 1)) {
    lcd.setCursor(0 , 2);
    lcd.print("          ");
    lcd.setCursor(0 , 2); lcd.print("Status:"); lcd.print("ON");
    value_uv1 = 5400;
    statusuv1 = 1;
    time_uv1 = millis();
  }
  /*============== PHÒNG UV1 END ====================== */

  /*============== PHÒNG UV2 START ====================== */
  if ((value_uv2 == 0) && (digitalRead (BT3) == 1)) {
    lcd.setCursor(10 , 2);
    lcd.print("          ");
    lcd.setCursor(10 , 2);
    lcd.setCursor(10 , 2); lcd.print("Status:"); lcd.print("ON");
    value_uv2 = 5400;
    statusuv2 = 1;
    time_uv2 = millis();
    
  }
  /*============== PHÒNG UV2 END ====================== */
  bigUV1 = value_uv1 / 60;
  bigUV2 = value_uv2 / 60;
  lcd_out();
  /*============ NẾU PHÒNG UV1 ĐANG BẬT VÀ PHÒNG UV2 ĐANG TẮT =========*/
  if ( (value_uv1 > 0) && (value_uv2 == 0) ) {
    Voltage = getVPP();
    VRMS = (Voltage / 2.0) * 0.707; //root 2 is 0.707
    AmpsRMS = ((VRMS * 1000) / mVperAmp) - 0.3; //0.3 is the error I got for my sensor
    Watt = (AmpsRMS * 220 / 1.2);
    digitalWrite(TRIAC1, HIGH);
    //    if ( AmpsRMS < 0.30 ){
    //      for (i = 1; i <= 20; i++) {
    //        digitalWrite(LOA, HIGH); delay(200); digitalWrite(LOA, LOW); delay(10);
    //      }
    //    }
    while((millis()-time_uv1) < 900) //sample for 1 Sec
   {
//    delay(900); 
    value_uv1--;
    if ( value_uv1 == 0){
      statusuv1 = 0;
    }
    time_uv1 = millis();
  }
  if ( (value_uv1 == 0) && (value_uv2 > 0) ) {
    while((millis()-time_uv2) < 900) //sample for 1 Sec
   {
//    delay(980); 
    value_uv2--;
    time_uv2 = millis();
   }
  }
  if ( (value_uv1 > 0) && (value_uv2 > 0) ) {
    while((millis()-time_uv1) < 900) //sample for 1 Sec
   {
//    delay(980); 
    value_uv1--; value_uv2--;
    if ( value_uv1 == 0){
      statusuv1 = 0;
    }
    if ( value_uv2 == 0){
      statusuv2 = 0;
    }
    time_uv1 = millis();
   }
  }
  if (( statusuv1 == 1)&& (gui == 0)){
    gui = 1;
    startuv1();
  }
  if (( statusuv1 == 0)&&(gui == 1)){
    gui = 0;
    enduv1();
  }
}
  }
void startuv1() {
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"True\",\r\n     \"timer\":\"90\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
  if (httpResponseCode > 0) {
    String response = http.getString();
    //        Serial.println(httpResponseCode);
    //        Serial.println(response);
  }
  else {
    //        Serial.print("Error on sending POST: ");
    //        Serial.println(httpResponseCode);
  }
  http.end();
}
void enduv1() {
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
  if (httpResponseCode > 0) {
    String response = http.getString();
    //        Serial.println(httpResponseCode);
    //        Serial.println(response);
  }
  else {
    //        Serial.print("Error on sending POST: ");
    //        Serial.println(httpResponseCode);
  }
  http.end();
}
void lcd_out() {
  lcd.setCursor(0, 0); lcd.print("PHONG UV1");
  lcd.setCursor(10, 0); lcd.print("PHONG UV2");
  /*============ LCD UV1 ==================*/
  lcd.setCursor(0, 1);
  lcd.print("Timer :"); lcd.print(bigUV1);
  lcd.setCursor(0, 2);
  lcd.print("Status:");
  lcd.setCursor(0, 3);
  lcd.print(" A :"); lcd.print(AmpsRMS);
  lcd.print(" ");
  /*============ LCD UV2 ==================*/
  lcd.setCursor(10, 1);
  lcd.print("Timer :"); lcd.print(bigUV2);
  lcd.setCursor(10, 2);
  lcd.print("Status:");
  lcd.setCursor(10, 3);
  lcd.print(" A :"); lcd.print(AmpsRMS);
  lcd.print(" ");
}

// ***** function calls ******
float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(ACS1);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the minimum sensor value*/
      minValue = readValue;
    }
  }
  // Subtract min from max
  result = ((maxValue - minValue) * 5) / 4096.0; //ESP32 ADC resolution 4096
  return result;
}
String getChipId()
{
  String ChipIdHex = String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);
  ChipIdHex += String((uint32_t)ESP.getEfuseMac(), HEX);
  return ChipIdHex;
}

void update_FOTA()
{
  String url = "http://otadrive.com/deviceapi/update?";
  url += "k=" + key;
  url += "&v=" + version;
  url += "&s=" + getChipId(); // định danh thiết bị trên Cloud

  WiFiClient client;
  httpUpdate.update(client, url, version);
}
