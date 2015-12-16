//
//  WiFiConfigurator.cpp
//  
//
//  Created by michael shaulov on 11/26/15.
//
//
#include <Arduino.h>
#include <config.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiConfigurator.h>


bool WiFiAPCaptive::__bConfigured = false;
IConfiguration* WiFiAPCaptive::__config = NULL;
ESP8266WebServer* WiFiAPCaptive::__WebServer = NULL;


WiFiAPCaptive::WiFiAPCaptive(String ssid,IConfiguration* pConfig) : _ssid(ssid)
{
    __config = pConfig;
}


void WiFiAPCaptive::start()
{
    configureAP();
    
    __bConfigured = false;
    
    // setup web server
    ESP8266WebServer webServer(80);
    __WebServer = &webServer;
    webServer.serveStatic("/",SPIFFS,"/index.html");
    webServer.serveStatic("/jquery.min.js",SPIFFS,"/jquery.min.js");
    webServer.on("/scanwifis", handleScanWifis);
    webServer.on("/configure", handleConfigure);
    webServer.begin();
    
    DEBUG("captive webserver started");
    
    // perform loop
    while(!__bConfigured)
    {
        delay(50);
        _dnsServer.processNextRequest();
        webServer.handleClient();
    }
    
    DEBUG("captive webserver ended");
    
    delay(1000);
}


// configure the access point
void WiFiAPCaptive::configureAP()
{
    DEBUG("initializing WiFi module to AP mode");
    /* Soft AP network parameters */
    IPAddress apIP(10, 0, 0, 1);
    IPAddress netMsk(255, 255, 255, 0);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(_ssid.c_str());
    
    
    // wait for setup
    delay(1000);
    
    DEBUG("AP IP address: ");
    DEBUG(WiFi.softAPIP());
    
    DEBUG("setting up DNS server");
    // redirect dns quries to local ip address
    _dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    _dnsServer.start(53, "*", apIP);
    
    DEBUG("setting up DNS server completed");
}

void WiFiAPCaptive::sendServerResponse(const char* responseStr, int errorCode)
{
    __WebServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    __WebServer->sendHeader("Pragma", "no-cache");
    __WebServer->sendHeader("Expires", "-1");
    __WebServer->send(errorCode, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    __WebServer->sendContent(responseStr);
    __WebServer->client().stop();
}

// handle configuration request
void WiFiAPCaptive::handleConfigure()
{
    DEBUG("Handling new captive configuration...");
    if(!__config->beginUpdate())
    {
        sendServerResponse("failed to update settings",500);
        return;
    }
    // save settings in file
    int totalArgs = __WebServer->args();
    for(int i =0; i < totalArgs; ++i)
    {
        String key = __WebServer->argName(i);
        String val = __WebServer->arg(i);
        DEBUG("saving: " + key + "=" + val);
        __config->updateValue(key.c_str(),val.c_str());
    }
    
    DEBUG("Captive configuration update completed");
    __config->endUpdate();
    // send successful response
    sendServerResponse("New settings saved",200);
    __bConfigured = true;
}


void WiFiAPCaptive::handleScanWifis()
{
    int n = WiFi.scanNetworks();

    String json = "{";
    
    for (int i = 0; i < n; ++i) {
        json += "\"" + String(i, DEC) + "\" : \"" + WiFi.SSID(i) + "\"";
        
        if(i < n-1)
        {
            json += ",";
        }
    }
    
    json += "}";
    
    DEBUG("WiFi Scann Json: " + json);
    
    sendServerResponse(json.c_str(),200);
}

