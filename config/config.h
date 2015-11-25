//
//  config.h
//  
//
//  Created by michael shaulov on 11/4/15.
//
//

#ifndef config_h
#define config_h

#include <Arduino.h>
#include "FS.h"

#define DEBUG(x) Serial.println(x)

/***
 * Configuration object interface
 ***/
class IConfiguration
{
public:
    virtual ~IConfiguration() {}
    virtual String getValue(const char* key) = 0;
    virtual bool beginUpdate() = 0;
    virtual void endUpdate() = 0;
    virtual void updateValue(const char* key, const char* val) = 0;
};

/***
 * Class to store and load configurations from a flat INI file
 ***/
class IniFile : public IConfiguration
{
public:
    IniFile(const char* path);
    
    virtual String getValue(const char* key);
    
    /*
        Function to begin the update process
     */
    virtual bool beginUpdate();
    
    virtual void endUpdate();
    
    virtual void updateValue(const char* key, const char* val);
    
    void clearAll();
    
    bool exists() {
        return SPIFFS.exists(_path);
    }
    
private:
    String _path;
    File fd;
};


#endif /* config_h */
