/*
  Original -> Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-post-ifttt-thingspeak-arduino/ 
  Modified -> Rechard Preston
  Complete project details at Complete project details at https://github.com/S1L3NTANG3L/Thingsboard-Pico-CPP/
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#define DHTPIN 1
#define DHTTYPE DHT11
const char* ssid = "SSID"; //NEEDED
const char* password = "PASSWORD";//NEEDED

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://IO.THINGSBOARD.COM:9090/api/v1/ACCESS_TOKEN/telemetry";//IP,PORT & ACCESS TOKEN NEEDED

// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /*  Code below is connected to sensor, change to what your using */
  float h = dht.readHumidity();  
  float t = dht.readTemperature();
  Serial.println("Temperature: " + String(t,2));  
  Serial.println("Humidity: " + String(h,2));
  //                             | JSON IN STRING FORM, JUST KEEP ADDING TELEMETRY VALUES AS NEEDED
  //                             V
  String httpRequestData = "{\"temperature\":\"" + String(t,2) + "\",\"humidity\":\"" + String(h,2) + "\"}"; 
  Serial.println(httpRequestData);  

  if(WiFi.status()== WL_CONNECTED){
      
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      // Specify content-type header
      //No need to add other headers but will get a HTTP Response 400 if server uses ssl -> Direct http connection works fine
      http.addHeader("Content-Type", "application/json");
      // Data to send with HTTP POST
         
         
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);*/
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println();
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    //delay(30000);//Send a reading every 30 seconds
    sleep_ms(30000);
}