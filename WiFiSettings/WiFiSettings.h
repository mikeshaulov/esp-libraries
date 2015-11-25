#ifndef WIFISETTINGS_H_
#define WIFISETTINGS_H_

#include <Arduino.h>
#include "FS.h"

/*
  Class to store settings of a WiFi
*/

#define CFG_FILE "/ap_settings.cfg"

class CWiFiSettings
{
  public:
    CWiFiSettings(){}

    CWiFiSettings(const String& APName, const String& APPass) : _APName(APName), _APPass(APPass){}
  
    // Getters
    const String& getAPName() const {return _APName;}
    const String& getAPPass() const {return _APPass;}
    // Setters
    void setAPName(const String& APName) {_APName = APName;}
    void setAPPass(const String& APPass) {_APPass = APPass;}


    bool load()
    {
        File f = SPIFFS.open(CFG_FILE, "r");
        // check if file was opened succesfuly
        if(!f) return false;

        _APName = f.readStringUntil('\n');
        _APPass = f.readStringUntil('\n');

        f.close();
        
        return true;
    }

    bool save()
    {
        File f = SPIFFS.open(CFG_FILE, "w");
        // check if file was opened succesfuly
        if(!f) return false;        

      
        f.write((const uint8_t*)_APName.c_str(), _APName.length());
        f.write('\n');
        f.write((const uint8_t*)_APPass.c_str(), _APPass.length());
        f.write('\n');

        f.close();
    }
    
  private:
    String _APName;
    String _APPass;
};
#endif
