//
//  WiFiUpdater.hpp
//  
//
//  Created by michael shaulov on 12/17/15.
//
//

#ifndef WiFiUpdater_hpp
#define WiFiUpdater_hpp

extern const char* CODE_VERSION; /* This should be declared in the main sketch file */

class WiFiUpdater
{
public:
    static void setup(ESP8266WebServer& webServer);
};

#endif /* WiFiUpdater_hpp */
