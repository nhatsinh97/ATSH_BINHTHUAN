#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> //v5.13.5
const char* ssid = "GreenFeed";
const char* password = "2020@GREENFEED";
String text;
int timer = 0;
void setup() {
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.println("Connected to the WiFi network");
}
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.32.238:58888/api/refresh_service"); //My docker
    http.addHeader("Content-Type", "application/json");
    if (Serial.available() > 0) {
      //      int val = Serial.read();
      text = Serial.readStringUntil('\n');
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
  }
else {
  Serial.println("Error in WiFi connection");
}
}
