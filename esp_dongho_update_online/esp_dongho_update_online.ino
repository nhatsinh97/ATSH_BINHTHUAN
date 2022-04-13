/* Chương trình giao tiếp API hiển thi thời gian và phát thông báo */
#include <Wire.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#define D5 14 // GPIO14 thông báo 30 phút
#define D6 12 // GPIO12 thông báo kết thúc
#define D7 13 // GPIO13 thông báo on
#define SERVER_IP "172.17.128.50:8089/api/Farm/getcountdownsecond"
#ifndef STASSID
#define STASSID "TOTOLINK N300RH"
#define STAPSK  ""
const char* ssid = STASSID;
const char* password = STAPSK;
int timer_uv = 0, gio = 0, phut = 0, giay = 0, dem = 0, load = 0, i = 0, starus = 0;
char a[100]     = "OFF  ";
char onn[100]   = "ON   ";
char yes[100]   = "YES  ";
char ozone[100] = "OZONE";
char uv[100]    = "UV   ";
#endif
LiquidCrystal_I2C lcd(0x27, 16, 2); //Thiết lập địa chỉ và loại LCD
void setup() {
  Serial.begin(115200);
  Wire.begin(D4, D3);              //Thiết lập chân kết nối I2C (SDA,SCL);
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

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  //ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  //   MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

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
    delay(1000);
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
    //    lcd.setCursor(0, 1);
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
  lcd.setCursor(0, 0); lcd.print("status: ");
  lcd.setCursor(0, 1); lcd.print("Timer: ");
}
void loop() {
  ArduinoOTA.handle();
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.clear(); lcd.print("Failed! Reboot");delay(1000); ESP.restart(); }
  if ((WiFi.status() == WL_CONNECTED)) { ArduinoOTA.handle();
    if ( starus == 1 ) { onuv();} else { offuv(); }
    if (gio == 1) {
      strcpy(a, ozone);
      lcd.setCursor(8, 0);
      lcd.print("      ");
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (phut > 30) {
      strcpy(a, ozone);
      lcd.setCursor(8, 0);
      lcd.print("      ");
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (phut > 0 && phut < 30 ) {
      strcpy(a, uv);
      lcd.setCursor(8, 0);
      lcd.print("      ");
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (phut == 30) {
      strcpy(a, yes);
      lcd.setCursor(8, 0);
      lcd.print("      ");
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (timer_uv > 0) {starus = 1;
      lcd.setCursor(8, 0);
      lcd.print("      ");
      lcd.setCursor(8, 0); lcd.print(a);
      lcd.setCursor(7, 1);
      lcd.print(gio); lcd.print(":");
      lcd.print(phut); lcd.print(":");
      lcd.print(giay); lcd.print("  ");
      lamcham(); check(); }
      else{
        strcpy(a, a);
        starus = 0;
        lamcham(); check();
      }
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
  for (i = 1 ; i <= 59 ; i++) {
    if (i > 9 ){
      lcd.setCursor(14, 0); lcd.print(i);
    }else{
      lcd.setCursor(14, 0); lcd.print(" ");
      lcd.setCursor(15, 0); lcd.print(i);
    }
    delay(900);
    ArduinoOTA.handle();
  }
}
void check() {
  ArduinoOTA.handle(); WiFiClient client; HTTPClient http;
  http.begin(client, "http://" SERVER_IP );
  http.addHeader("Content-Type", "application/json");
  lcd.setCursor(8, 0); lcd.print("     ");
  lcd.setCursor(8, 0); lcd.print("check");
  int httpCode = http.POST("{\"mac_address\":\"6c:1c:71:5c:9b:31\"}");
  if (httpCode > 0) {
    lcd.setCursor(8, 0); lcd.print("     ");
    lcd.setCursor(8, 0); lcd.print(httpCode); delay(250);
    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      timer_uv  = payload.toInt (); // Chuyển string thành int
      gio  = timer_uv / 3600;
      phut = timer_uv % 3600 / 60;
      giay = timer_uv % 3600 % 60;
      lcd.setCursor(8, 0);
      lcd.print("      ");
      lcd.setCursor(8, 0); lcd.print(a);
      lcd.setCursor(7, 1);
      lcd.print(gio); lcd.print(":");
      lcd.print(phut); lcd.print(":");
      lcd.print(giay); lcd.print("  ");
    }
  } else {
    lcd.setCursor(8, 0); lcd.print("      ");
    lcd.setCursor(8, 0); lcd.print("failed"); delay(250);
    //, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
