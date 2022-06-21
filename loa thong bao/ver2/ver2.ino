/* Chương trình giao tiếp API hiển thi thời gian và phát thông báo */
#include <Wire.h>
#include "time.h"
#include <ESP8266WiFiMulti.h>         // Thư viện dùng để kết nối WiFi đa cấu hình của ESP8266
ESP8266WiFiMulti wifiMulti;           // Khởi tạo đối tượng wifiMulti từ ESP8266WiFiMulti
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
#define SERVER_IP "172.17.128.50:58185/api/Farm/getcountdownsecond"  //http://172.17.128.50:58185
int timer_uv = 0, dem = 0, load = 0, i = 0, starus = 0, big = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); //Thiết lập địa chỉ và loại LCD
BigNumbers_I2C bigNum(&lcd); // construct BigNumbers_I2C object, passing to it the name of our LCD object
byte x = 7;//x & y determines position of character on screen
byte y = 0;
void update_FOTA();
uint32_t updateCounter = 0;
String version = "2.3";
String key = "37f0d021-9105-4822-8304-fed5b365c89d";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;
String gio, phut, giay;
void setup() {
  Serial.begin(115200);
  Wire.begin(D4, D3);              //Thiết lập chân kết nối I2C (SDA,SCL);
  bigNum.begin(); // set up BigNumbers
  lcd.init();                      //Khởi tạo LCD
  lcd.backlight();                 //Bật đèn nền
  wifiMulti.addAP("GreenFeed", "2020@GREENFEED");        // Kết nối vào mạng WiFi ở nhà
  wifiMulti.addAP("TOTOLINK N300RH", "");     // Kết nối vào mạng WiFi ở công ty
  Serial.println("Connecting ...");
  // Chờ kết nối WiFi được thiết lập. Quét tìm mạng WiFi và kết nối với mạng mạnh nhất.
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());        // Cho biết tên mạng WiFi đã kết nối thành công
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());     // Gởi địa chỉ IP đến máy tinh
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.print("Failed! Reboot");
    delay(5000);
    ESP.restart();
  }
  pinMode(D5, OUTPUT); pinMode(D6, OUTPUT); pinMode(D7, OUTPUT);
  digitalWrite(D5, HIGH); digitalWrite(D6, HIGH); digitalWrite(D7, HIGH);
  delay(1000); lcd.clear();
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}
void loop() {
  ///////////////////boot////////////
  //  WiFiManager manager;
  //  updateCounter++;
  //  if (updateCounter > 150)
  if (gio ==  "18" && phut == "50" && giay == "0")
  {
    //    updateCounter = 0;
    Serial.println("Check update");
    //    update_FOTA();
  }
  /////////////////end boot////////////
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.clear(); lcd.print("Failed! Reboot"); delay(1000); ESP.restart();
  }
  if ((WiFi.status() == WL_CONNECTED)) {
//    Serial.print(gio); Serial.print("/"); Serial.print(phut); Serial.print("/"); Serial.println(giay);
    printLocalTime();
    if ( starus == 1 ) {
      onuv();
    } else {
      offuv();
    }
    if (timer_uv > 0) {
      starus = 1;
      lamcham(); check();
    }
    else {
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
  for (i = 60 ; i >= 0 ; i--) {
    big  = timer_uv / 60;
    gio  = timer_uv / 3600;
    phut = timer_uv % 3600 / 60;
    giay = timer_uv % 3600 % 60;
    bigNum.displayLargeInt(big, x, y, 2, false);
    lcd.setCursor(13 , 1); lcd.print("."); lcd.print(i); lcd.print(" ");
    Serial.println(i);
    delay(900);
    ArduinoOTA.handle();
  }
}
void check() {
  WiFiClient client; HTTPClient http;
  http.begin(client, "http://" SERVER_IP );
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"mac_address\":\"6c:1c:71:5c:9b:31\"}");//6c:1c:71:5c:9b:31 dc:a6:32:0f:bd:ac
  if (httpCode > 0) {
    Serial.print(httpCode); delay(250);
    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      timer_uv  = payload.toInt (); // Chuyển string thành int
      if (timer_uv  == 0) {
        starus = 0;
      }
    }
  }
  http.end();
}


void update_FOTA()
{
  WiFiClient client; HTTPClient http;
  String url = "http://otadrive.com/deviceapi/update?";
  url += "k=" + key;
  url += "&v=" + version;
  url += "&s=" ;
  url += "esp8266123"; // định danh thiết bị trên Cloud

//  WiFiClient client;
  httpUpdate.update(client, url, version);
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  //  Serial.println(&timeinfo,"%A, %d/%B/%Y %H:%M:%S");
  gio = timeinfo.tm_hour;
  phut = timeinfo.tm_min;
  giay = timeinfo.tm_sec;
  //  int result = val.toInt ();
}
