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
#include <WiFiUpdater.h>
#include <switch.h>

bool WiFiConfigurator::__bConfigured = false;
IConfiguration* WiFiConfigurator::__config = NULL;
ESP8266WebServer* WiFiConfigurator::__WebServer = NULL;


WiFiConfigurator::WiFiConfigurator(String ssid,IConfiguration* pConfig) : _ssid(ssid)
{
    __config = pConfig;
}


void WiFiConfigurator::startCaptive()
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
    WiFiUpdater::setup(webServer);
    
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
void WiFiConfigurator::configureAP()
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

void WiFiConfigurator::sendServerResponse(const char* responseStr, int errorCode)
{
    __WebServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    __WebServer->sendHeader("Pragma", "no-cache");
    __WebServer->sendHeader("Expires", "-1");
    __WebServer->send(errorCode, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    __WebServer->sendContent(responseStr);
    __WebServer->client().stop();
}

// handle configuration request
void WiFiConfigurator::handleConfigure()
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


void WiFiConfigurator::handleScanWifis()
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

void WiFiConfigurator::connectToWiFi(CBaseSwitch* pLed)
{
    String wifiName = __config->getValue("WIFI_NAME");
    String wifiPassword = __config->getValue("WIFI_PASSWORD");
    wifiName.trim();
    wifiPassword.trim();
    
    DEBUG("connecting to WiFi: [" + wifiName + "," + wifiPassword + "]");
    // connect to WiFi
    WiFi.mode(WIFI_STA);
    if(!wifiPassword.length())
    {
        WiFi.begin(wifiName.c_str());
    }
    else{
        WiFi.begin(wifiName.c_str(), wifiPassword.c_str());
    }
    
    
    int retries = 0;
    bool ledState = false;
    
    // loop until we are conected to the WiFi
    while ((WiFi.status() != WL_CONNECTED) && (retries++ < MAX_WIFI_CONNECT_RETRY)) {
        
        if(pLed)
        {
            ledState = !ledState;
            pLed->TurnBool(ledState);
        }
        delay(500);
        Serial.print(".");
    
        // every small interval wait pause for a while, as the router might expiriencing downtime
        if((retries % MAX_WIFI_CONNECT_RETRY_SMALL_INTERVALS) == 0)
            delay(MAX_WIFI_CONNECT_RETRY_INTERVAL_DELAY);
    }
    
    if(WiFi.status() != WL_CONNECTED)
    {
        DEBUG("max retries reached, clearing settings and restarting");
        __config->clearAll();
        ESP.restart();
    }
    
    // reset reconnect counter
    _reconnect_retry = 0;
    
    DEBUG("WiFi connected");
    DEBUG("IP address: ");
    DEBUG(WiFi.localIP());

    if(pLed)
    {
        pLed->TurnOff();
    }
}

bool WiFiConfigurator::checkWiFiConnectivity(CBaseSwitch* pErrorLed)
{
    if(WiFi.status() != WL_CONNECTED)
    {
        if(pErrorLed)
            pErrorLed->TurnOn();
        
        DEBUG("WiFi connection lost - restarting ESP, [status] " + String(WiFi.status(), DEC));
        if(_reconnect_retry++ > MAX_WIFI_RECONNECT_RETRY)
        {
            // restart to initiate reconnecting loop
            ESP.restart();
        }
        return false;
    }
    
    _reconnect_retry = 0;
    return true;
}
