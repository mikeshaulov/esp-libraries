//
//  WiFiConfigurator.h
//  
//
//  Created by michael shaulov on 11/4/15.
//
//

#ifndef WiFiConfigurator_h
#define WiFiConfigurator_h

/*
 Creates an acceess point with with a captive configuration page
 configration page is loaded from /index.html
 */
class WiFiAPCaptive
{
public:
    /*
     setup an access point to configure the device
     @param ssid        the name of the access point
     @param pConfig     configuration handler
     */
    WiFiAPCaptive(String ssid,IConfiguration* pConfig);
    
    /*
     start the access point
     */
    void start();
    
private:
    
    // configure the access point
    void configureAP();
    
    static void sendServerResponse(const char* responseStr, int errorCode = 200);
    
    // handle configuration request
    static void handleConfigure();
    
    // handles scanned WiFis request
    static void handleScanWifis();
    
    
    String _ssid;
    
    /* DNS Server */
    DNSServer _dnsServer;
    // used by web-server handler
    static ESP8266WebServer* __WebServer;
    static bool __bConfigured;
    static IConfiguration* __config;

};

#endif /* WiFiConfigurator_h */
