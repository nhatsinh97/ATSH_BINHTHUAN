/* Chương trình giao tiếp API hiển thi thời gian và phát thông báo */
#include <Wire.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#define D5 14 // GPIO14 thông báo 30 phút
#define D6 12 // GPIO12 thông báo kết thúc
#define D7 13 // GPIO13 thông báo on
//#define SERVER_IP "172.17.128.50:58185/api/Farm/getcountdownsecond"
#define SERVER_IP "172.17.128.50:58185/api/Farm/checkalarm60"
#ifndef STASSID
#define STASSID "baove"
#define STAPSK  ""
const char* ssid = STASSID;
const char* password = STAPSK;
int timer_uv = 0, gio = 0, phut = 0, giay = 0, dem = 0, load = 0, i = 0, starus = 0, big = 0;
char a[100]     = "OFF  ";
char onn[100]   = "ON   ";
char yes[100]   = "YES  ";
char ozone[100] = "OZONE";
char uv[100]    = "UV   ";
#endif
LiquidCrystal_I2C lcd(0x27, 16, 2); //Thiết lập địa chỉ và loại LCD
BigNumbers_I2C bigNum(&lcd); // construct BigNumbers_I2C object, passing to it the name of our LCD object
byte x = 7;//x & y determines position of character on screen
byte y = 0;
void setup() {
  Serial.begin(115200);
  Wire.begin(D4, D3);              //Thiết lập chân kết nối I2C (SDA,SCL);
  bigNum.begin(); // set up BigNumbers
  lcd.init();                      //Khởi tạo LCD
  lcd.backlight();                 //Bật đèn nền
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  lcd.print("ket noi wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 1);
    delay(200);
    lcd.print(">");
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.print("Failed! Reboot");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    lcd.clear();                     //Xóa màn hình
    lcd.setCursor(0, 0);
    lcd.print("Start updating ");
    lcd.setCursor(0, 1);
    lcd.print(type);
    delay(500);
  });
  ArduinoOTA.onEnd([]() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("      End    ");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("update code");
    lcd.setCursor(0, 1);
    lcd.print("Progress: ");
    lcd.print(progress / (total / 100));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error: ");
    lcd.setCursor(0, 1);
    lcd.print(error);
    if (error == OTA_AUTH_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Receive Failed");
    }
    else if (error == OTA_END_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("End Failed");
    }
  });
  ArduinoOTA.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP address: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  pinMode(D5, OUTPUT); pinMode(D6, OUTPUT); pinMode(D7, OUTPUT);
  digitalWrite(D5, HIGH); digitalWrite(D6, HIGH); digitalWrite(D7, HIGH);
  delay(1000); lcd.clear();
}
void loop() {
  ArduinoOTA.handle();
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Mat ket noi"); delay(500); ESP.restart();
  }
  if ((WiFi.status() == WL_CONNECTED)) {
    ArduinoOTA.handle();
    lcd.setCursor(0, 0); lcd.print("Status:");
    lcd.setCursor(2, 1); lcd.print(a);
    if ( starus == 1 ) {
      onuv();
    } else {
      offuv();
    }
    lamcham();
    check();
  }
}
void onuv() {
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  delay(100);
  digitalWrite(D7, LOW);
}
void offuv() {
  digitalWrite(D5, HIGH);
  digitalWrite(D7, HIGH);
  delay(100);
  digitalWrite(D6, LOW);
}
void lamcham() {
  for (i = 0 ; i <= 300 ; i++) {
    bigNum.displayLargeInt(big, x, y, 2, false);
    lcd.setCursor(13 , 1); lcd.print("."); lcd.print(i); lcd.print(" ");
    if ((starus == 1) && (i == 60)) {
      big++;
    }
    Serial.println(i);
    delay(900);
    ArduinoOTA.handle();
  }
}
void check() {
  ArduinoOTA.handle(); WiFiClient client; HTTPClient http;
  http.begin(client, "http://" SERVER_IP );
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"mac_address\":\"6c:1c:71:5c:9b:31\"}");//6c:1c:71:5c:9b:31 dc:a6:32:0f:bd:ac
  if (httpCode > 0) {
    Serial.print(httpCode); delay(250);
    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      //      timer_uv  = payload.toInt (); // Chuyển string thành int
      if (payload  == "No") {
        big = 0;
        strcpy(a, "OFF");
        starus = 0;
      }
      else {
        strcpy(a, "ON ");
        starus = 1;
      }
    }
  }
  http.end();
}
