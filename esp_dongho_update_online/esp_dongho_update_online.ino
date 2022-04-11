#include <ESP8266WiFiMulti.h>
#include <Wire.h>                 //Thư viện giao tiếp I2C
#include <LiquidCrystal_I2C.h>    //Thư viện LCD
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#define D5 14 // GPIO14
#define D6 12 // GPIO12
#define D7 13 // GPIO13
#define SERVER_IP "172.17.128.50:8089/api/Farm/getcountdownsecond"
#ifndef STASSID
#define STASSID "TOTOLINK N300RH"
#define STAPSK  ""
const char* currentVersion = "1.0";
const char* serverUrl = "http://192.168.32.98/firmware/esp_dongho_update_online.ino.nodemcu.bin";
const char* ssid = STASSID;
const char* password = STAPSK;
int timer_uv = 0;
int gio = 0;
int phut = 0;
int giay = 0;
int dem = 0;
int load = 0;
char a[100] = "OFF";
char onn[100]   = "ON   ";
char yes[100]   = "YES  ";
char ozone[100] = "OZONE";
char uv[100]    = "UV   ";
LiquidCrystal_I2C lcd(0x27, 16, 2); //Thiết lập địa chỉ và loại LCD
#endif
void setup() {
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
  Serial.begin(115200);
  Wire.begin(D4, D3);              //Thiết lập chân kết nối I2C (SDA,SCL);
  lcd.init();                      //Khởi tạo LCD
  lcd.clear();                     //Xóa màn hình
  lcd.backlight();                 //Bật đèn nền
  lcd.setCursor(2, 0);             //Đặt vị trí ở ô thứ 3 trên dòng 1
  lcd.print("Welcom to...");          //Ghi đoạn text "Welcom to"
  lcd.setCursor(0, 1);
  lcd.print("FARM BINH THUAN");
  delay(1000); lcd.clear();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  lcd.clear();                     //Xóa màn hình
  lcd.setCursor(0, 0);
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
    lcd.print("dang nap code");
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
    } else if (error == OTA_BEGIN_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("End Failed");
    }
  });
  ArduinoOTA.begin();
  lcd.clear();                     //Xóa màn hình
  lcd.setCursor(0, 0);
  lcd.print("IP address: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());



}

void loop() {
  ArduinoOTA.handle();
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.print("Failed! Reboot");
    delay(5000);
    ESP.restart();
  }
  if ((WiFi.status() == WL_CONNECTED)) {
    ArduinoOTA.handle();
    gio = timer_uv / 3600;
    phut = timer_uv % 3600 / 60;
    giay = timer_uv % 3600 % 60;//alarm
    if (timer_uv == 0) {
      load = 0;
      delay(1000);
      ArduinoOTA.handle();
      delay(10000);
      check();
    }
    if (gio == 1) {
      digitalWrite(D5, HIGH);
      digitalWrite(D6, HIGH); delay(500);
      digitalWrite(D7, LOW);
      strcpy(a, ozone);
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (phut > 30) {
      digitalWrite(D5, HIGH);
      digitalWrite(D6, HIGH); delay(500);
      digitalWrite(D7, LOW);
      strcpy(a, ozone);
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (phut > 0 && phut < 30 ) {
      strcpy(a, uv);
      lcd.setCursor(8, 0);
      lcd.print(a);
    }
    if (phut == 30) {
//      digitalWrite(D6, HIGH);
//      digitalWrite(D7, HIGH); delay(500);
//      digitalWrite(D5, LOW);
      strcpy(a, yes);
      lcd.setCursor(7, 0);
      lcd.print(a);
    }
    if (timer_uv > 0) {

      lcd.setCursor(0, 0);
      lcd.print("starus: ");
      lcd.setCursor(8, 0);
      lcd.print(a);
      lcd.setCursor(0, 1);
      lcd.print("Timer: ");
      lcd.setCursor(7, 1);
      lcd.print(gio); lcd.print(":");
      lcd.print(phut); lcd.print(":");
      lcd.print(giay);lcd.print("  ");
      lcd.setCursor(15, 1);
      lcd.print(dem);
      delay(1000);
      timer_uv--;
      load++;
//      if (load == 120){
//        load = 0;
//        check();
//      }
      if ( timer_uv == 0) {
        digitalWrite(D5, HIGH);
        digitalWrite(D7, HIGH); delay(500);
        digitalWrite(D6, LOW);
      }
      //lcd.clear();                     //Xóa màn hình
      ArduinoOTA.handle();
    }
  }
}
void check() {
  ArduinoOTA.handle();
  //lcd.clear();                     //Xóa màn hình
  WiFiClient client;
  HTTPClient http;
  //lcd.setCursor(0, 0);
  // configure traged server and url
  http.begin(client, "http://" SERVER_IP ); //HTTP
  http.addHeader("Content-Type", "application/json");
  //lcd.print("starus:");
  lcd.setCursor(8, 0);
  lcd.print("     ");
  lcd.setCursor(8, 0);
  lcd.print("check");
  int httpCode = http.POST("{\"mac_address\":\"6c:1c:71:5c:9b:31\"}");
  if (httpCode > 0) {
    lcd.setCursor(8, 0);
    lcd.print("     ");
    lcd.setCursor(8, 0);
    lcd.print(httpCode);
    delay(500);
    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      timer_uv  = payload.toInt (); // Chuyển string thành int
      lcd.setCursor(0, 1);
      lcd.print("Timer: ");
      lcd.setCursor(7, 1);
      lcd.print(gio); lcd.print(":");
      lcd.print(phut); lcd.print(":");
      lcd.print(giay);lcd.print("  ");
      lcd.setCursor(15, 1);
      lcd.print(dem);
      digitalWrite(D5, HIGH);
      digitalWrite(D6, HIGH); delay(500);
      digitalWrite(D7, HIGH);
      delay(500);
      lcd.clear();
      if (timer_uv == 0) {
        lcd.print("starus: "); lcd.setCursor(8, 0);
        lcd.print("off");
        lcd.setCursor(0, 1);
        lcd.print("Timer: ");
        lcd.setCursor(7, 1);
        lcd.print(gio); lcd.print(":");
        lcd.print(phut); lcd.print(":");
        lcd.print(giay);
        lcd.setCursor(15, 1);
        lcd.print(dem);
        delay(500);
      }
      if (timer_uv > 0) {
        dem++; if (dem == 10) {
          dem = 1;
        }
        lcd.print("starus:"); lcd.setCursor(0, 8);
        lcd.setCursor(0, 1);
        lcd.print("Timer: ");
        lcd.setCursor(7, 1);
        lcd.print(gio); lcd.print(":");
        lcd.print(phut); lcd.print(":");
        lcd.print(giay);
        lcd.setCursor(15, 1);
        lcd.print(dem);
        delay(500);
      }
    }
  } else {
    lcd.clear();
    lcd.print("POST... failed");
    //, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
