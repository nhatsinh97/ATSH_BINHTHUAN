#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>
#include <Wire.h>
#include <ArduinoJson.h> //v5.13.5
#include <HTTPClient.h>
#include <WiFiMulti.h>         // Thư viện dùng để kết nối WiFi đa cấu hình của ESP8266
#include <Arduino.h>
#include <HTTPUpdate.h>
#include "time.h"
//#include <WiFiManager.h>
WiFiMulti wifiMulti;           // Khởi tạo đối tượng wifiMulti từ ESP8266WiFiMulti
void update_FOTA();
LiquidCrystal_I2C lcd(0x27, 20, 4); //Thiết lập địa chỉ và loại LCD
BigNumbers_I2C bigNum(&lcd); // construct BigNumbers_I2C object, passing to it the name of our LCD object
byte x = 7;//x & y determines position of character on screen
byte y = 1;
uint32_t updateCounter = 0;
String version = "2.4";
String key = "37f0d021-9105-4822-8304-fed5b365c89d";
int i;
String text;
int timer = 0;
const int sensorIn = 4;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 185;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
unsigned long time1 = 0;
unsigned long time2 = 0;

byte TRIAC3 = 16;
byte LED = 2;
void setup()
{
  Serial.begin(115200);
  Wire.begin();              //Thiết lập chân kết nối I2C (SDA,SCL);
  bigNum.begin(); // set up BigNumbers
  lcd.init();                      //Khởi tạo LCD
  lcd.backlight();                 //Bật đèn nền
  lcd.setCursor(0, 0);
  lcd.print(" Connecting ... ");
  wifiMulti.addAP("baove", "");
  wifiMulti.addAP("TOTOLINK N300RH", "");
  wifiMulti.addAP("GreenFeed", "2020@GREENFEED");
  // Chờ kết nối WiFi được thiết lập. Quét tìm mạng WiFi và kết nối với mạng mạnh nhất.
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    bigNum.displayLargeInt(i, x, y, 2, false);
    i++;
    if (i >= 50) {
      esp_restart();
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected to :");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.SSID());        // Cho biết tên mạng WiFi đã kết nối thành công
  lcd.setCursor(2, 2);
  lcd.print("IP address: ");
  lcd.setCursor(0, 3);
  lcd.print(WiFi.localIP());     // Gởi địa chỉ IP đến máy tinh
  //  pinMode(5, OUTPUT);       // dính đòn
  //  pinMode(6, INPUT_PULLUP); // dính đòn
  //  pinMode(8, OUTPUT);       // dính đòn
  //  pinMode(9, INPUT_PULLUP); // dính đòn
  //  pinMode(10, INPUT_PULLUP);// dính đòn
  //  pinMode(11, INPUT_PULLUP);// dính đòn
  //  pinMode(12, OUTPUT);      // dính đòn
  //  pinMode(16, OUTPUT);
  pinMode(TRIAC3, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(34, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
  delay(10000);
  lcd.clear();
}
void loop()
{
  Voltage = getVPP();
  VRMS = (Voltage / 2.0) * 0.707; //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000) / mVperAmp) - 0.3; //0.3 is the error I got for my sensor
  lcd.setCursor(0, 0);
  lcd.print(AmpsRMS);
  Watt = (AmpsRMS * 240 / 1.2);
  lcd.print(" ");
  lcd.print(Watt);
  lcd.print(" Watts");
  lcd.setCursor(0, 1);
  bigNum.displayLargeInt(updateCounter, x, y, 2, false);
  updateCounter++; delay(500);
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  if ( (unsigned long) (millis() - time1) > 2000 )
  {
    if ( digitalRead(TRIAC3) == LOW )
    {
      digitalWrite(TRIAC3, HIGH);
    } else {
      digitalWrite(TRIAC3, LOW );
    }
    time1 = millis();
  }

  if ( (unsigned long) (millis() - time2) > 200  )
  {
    if ( digitalRead(LED) == LOW )
    {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW );
    }
    time2 = millis();
  }
  delay (100);
  if (Serial.available() > 0) {
    //      int val = Serial.read();
    text = Serial.readStringUntil('\n');
    lcd.setCursor(0, 2);
    lcd.print(text);
    if (text == "60") {
      timer = 60;
      int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"True\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
      delay(20000);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      text = "";
    }
    if (text == "90") {
      timer = 90;
      int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"True\",\r\n     \"timer\":\"90\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
      delay(20000);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      text = "";
    }

    if (text == "0") {
      int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
      delay(20000);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      text = "";
    }
    if (text == "checkout") {
      int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"True\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
      delay(10000);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      text = "";
    }
    //  if (updateCounter == 200000)
    //    //  if (gio ==  "19" )
    //  {
    //    updateCounter = 0;
    //    Serial.println("Check update");
    //    update_FOTA();
    //  }
    /////////////////end boot////////////

    while (wifiMulti.run() != WL_CONNECTED) {
      delay(500);
      Serial.println(i);
      i++;
      if (i >= 50) {
        esp_restart();
      }
    }
  }
  //  digitalWrite(12, HIGH);   // bật đèn led
  //  delay(5);                  // dừng trong 1 giây
  //  digitalWrite(12, LOW);    // tắt đèn led
//  lcd.clear();
}
// ***** function calls ******
float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
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
   result = ((maxValue - minValue) * 5)/4096.0; //ESP32 ADC resolution 4096
      
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
