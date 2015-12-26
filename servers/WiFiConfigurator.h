//
//  WiFiConfigurator.h
//  
//
//  Created by michael shaulov on 11/4/15.
//
//

#ifndef WiFiConfigurator_h
#define WiFiConfigurator_h

#define MAX_WIFI_CONNECT_RETRY 1000
#define MAX_WIFI_CONNECT_RETRY_SMALL_INTERVALS 100
#define MAX_WIFI_CONNECT_RETRY_INTERVAL_DELAY 30000
#define MAX_WIFI_RECONNECT_RETRY 500

class CBaseSwitch;

/*
 Creates an acceess point with with a captive configuration page
 configration page is loaded from /index.html
 */
class WiFiConfigurator
{
public:
    /*
     setup an access point to configure the device
     @param ssid        the name of the access point
     @param pConfig     configuration handler
     */
    WiFiConfigurator(String ssid,IConfiguration* pConfig);
    
    /*
     start the access point
     */
    void startCaptive();
    
    /**
     tries to connect to the WiFi, if failed will clear the configuration and reboot
     @param pLed    led to blink during configuration
     @param maxRetries  maximum number of retries
     */
    void connectToWiFi(CBaseSwitch* pLed = NULL, int maxRetries = MAX_WIFI_CONNECT_RETRY);
    
    /**
     starts MDNS services based on configuration
     */
    void startMDNS();
    
    
    /**
    Checks if the WiFi is connected or not, and if not will wait a while and then if still not connected - reset
     @param pErrorLed   led to blink when there is no connectivity
     */
    bool checkWiFiConnectivity(CBaseSwitch* pErrorLed);
    
private:
    
    // configure the access point
    void configureAP();
    
    static void sendServerResponse(const char* responseStr, int errorCode = 200);
    
    // handle configuration request
    static void handleConfigure();
    
    // handles scanned WiFis request
    static void handleScanWifis();
    
    static void urldecode2(char *dst, const char *src, int maxSize);
    
    // the SSID for the settings captive portal
    String _ssid;
    
    /* DNS Server */
    DNSServer _dnsServer;
    // used by web-server handler
    static ESP8266WebServer* __WebServer;
    // if configured
    static bool __bConfigured;
    // configuration provider
    static IConfiguration* __config;
    // number of retries to reconnect
    int _reconnect_retry;

};

#endif /* WiFiConfigurator_h */
