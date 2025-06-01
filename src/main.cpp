#include <HTTPClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "env.h"



void setup()
{
  Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    //WiFi.begin(SSID, PASS);

    if (IS_WOKWI) 
    WiFi.begin(SSID, PASS, CHANNEL);
    else 
    WiFi.begin(SSID, PASS);
   
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(200);
        digitalWrite(LED_PIN, HIGH);
        Serial.print(".");
    }

    digitalWrite(LED_PIN, LOW);    
    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED);
    {
      //section for receiving light status
      String path_light = "/api/light";
      HTTPClient http;
      http.begin(ENDPOINT + path_light);
      http.addHeader("api-key", API_KEY1);

      int responseCode = http.GET();

      if (responseCode <= 0)
      {
        Serial.print("An error has occured with the code: ");
        Serial.println(responseCode);
        http.end();
        return;
      }

      String response = http.getString();
      Serial.print("HTTP Response Code");
      Serial.println(responseCode);
      Serial.println(response);
      http.end();

      JsonDocument object;

      DeserializationError error = deserializeJson(object, response);

      if (error)
      {
        Serial.println("Deserialization error");
        Serial.println(error.c_str());
        return;
      }

      bool light = object["light"];

      if(light == true)
      {
        digitalWrite(LED_PIN, HIGH);
      }
      else
      {
        digitalWrite(LED_PIN, LOW);
      }
      

    ///section for temperature sending

    String path_temp = "/api/temp";
    http.begin(ENDPOINT + path_temp);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("api-key", API_KEY2);

    pinMode(TEMP_PIN, INPUT);


    String request_body;
    float temp = analogRead(TEMP_PIN);
    //Serial.println(temp); Check to ensure a value is being read

    
    object["temp"] = temp; //digitalRead(LIGHT_PIN); //uses a function to determine the condition of the light and use that to activate the function

    object.shrinkToFit(); //optional, used to conserve data on the server

    serializeJson(object, request_body);

    int status_code = http.PUT(request_body); // or http.POST for posting

    if (status_code <= 0)
    {
      Serial.print("An error occured. See the status code: ");
      Serial.println(status_code);

      http.end(); //conserves energy on the server
      return;
    }

    Serial.print("Status code: ");
    Serial.println(status_code);

    String response_body = http.getString();
    Serial.println(response_body);
    http.end();


    }
    
}