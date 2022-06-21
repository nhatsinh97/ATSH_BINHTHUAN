#include "time.h"
#include <WiFiMulti.h>         // Thư viện dùng để kết nối WiFi đa cấu hình của ESP8266
WiFiMulti wifiMulti;           // Khởi tạo đối tượng wifiMulti từ ESP8266WiFiMulti

//#include <WiFiManager.h>
#include <Arduino.h>
#include <HTTPUpdate.h>
void update_FOTA();
uint32_t updateCounter = 0;
String version = "2.3";
String key = "37f0d021-9105-4822-8304-fed5b365c89d";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;
String gio, phut, giay;
void setup()
{
  Serial.begin(115200);
  Serial.println("123 go!");
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
  //  WiFiManager manager;
  //  bool success = manager.autoConnect("ESP32_AP", "password");
  //  if (!success) {
  //    Serial.println("Failed to connect");
  //  }
  //  else {
  //    Serial.println("Connected");
  //  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();


}
void loop()
{ ///////////////////boot////////////
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
  Serial.print("Ver: ");
  Serial.println(version);
  Serial.print(gio); Serial.print("/"); Serial.print(phut); Serial.print("/"); Serial.println(giay);
  printLocalTime();
  delay(500);

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
