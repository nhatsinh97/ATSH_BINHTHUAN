#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
const char* ssid = "GreenFeed";
const char* password =  "2020@GREENFEED";
int i = 0;
#define GPIO 25
char congbaove[10] = "xevao";
char out[40] = "";
void setup() {
  Serial.begin(115200);
  delay(40);   //Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  pinMode(GPIO, INPUT);
}
void loop() {
  WiFiClient client;
  HTTPClient http;
  if (digitalRead(GPIO) == 1 ){
    strcpy(congbaove, "xera");
  }else{
    strcpy(congbaove, "xevao");
  }
  String datagui = "{\"congbaove\":\"";
  String out = datagui +(congbaove) + "}";
  Serial.println(datagui +(congbaove) + "}");
  if (congbaove > 0){
    http.begin( "http://192.168.151.21:58888/api/refresh_service"  ); //http://172.17.128.50:58185/api/Farm/getcountdownsecond
    http.addHeader("Content-Type", "application/json");
    String out = datagui +(congbaove)+ "\""  + "}";
    int httpCode = http.POST(out); //+ " HTTP/1.1"
    delay(5000);
  }
//  for ( i = 1; i <= 100; i++) {
//    Serial.println(i);
//    delay(1000);
//    if (i == 100) {
//      gui();
//    }
//  }
}
void gui() {
  WiFiClient client;
  HTTPClient http;
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status//http://192.168.32.238:58888/api/refresh_service
    http.begin( "http://192.168.32.239:58888/api/refresh_service"  ); //http://172.17.128.50:58185/api/Farm/getcountdownsecond
    http.addHeader("Content-Type", "application/json");
    String json = "{\"congbaove\":\"xevao\"}"; //"{\"congbaove\":\"dc:a6:32:0f:bd:ac\"}"
    int httpCode = http.POST(out); //+ " HTTP/1.1"
    Serial.println(httpCode);
    if (httpCode > 0) {
//      Serial.println(httpCode);
      if (httpCode == HTTP_CODE_OK) {
        Serial.print(httpCode);
      }
    } else {
      Serial.print("loi roi");
    }
    http.end();
  }
}
