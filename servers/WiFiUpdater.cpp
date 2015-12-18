//
//  WiFiUpdater.cpp
//  
//
//  Created by michael shaulov on 12/17/15.
//
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>

#include "WiFiUpdater.h"

void WiFiUpdater::setup(ESP8266WebServer& webServer)
{
    webServer.onFileUpload([&webServer](){
        if(webServer.uri() != "/update") return;
        HTTPUpload& upload = webServer.upload();
        if(upload.status == UPLOAD_FILE_START){
            Serial.setDebugOutput(true);
            WiFiUDP::stopAll();
            Serial.printf("Update: %s\n", upload.filename.c_str());
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if(!Update.begin(maxSketchSpace)){//start with max available size
                Update.printError(Serial);
            }
        } else if(upload.status == UPLOAD_FILE_WRITE){
            if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
                Update.printError(Serial);
            }
        } else if(upload.status == UPLOAD_FILE_END){
            if(Update.end(true)){ //true to set the size to the current progress
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
            Serial.setDebugOutput(false);
        }
        yield();
    });
    
    webServer.on("/update", HTTP_POST, [&webServer](){
        webServer.sendHeader("Connection", "close");
        webServer.sendHeader("Access-Control-Allow-Origin", "*");
        webServer.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
        ESP.restart();
    });
}