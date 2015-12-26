//
//  config.cpp
//  
//
//  Created by michael shaulov on 11/26/15.
//
//

#include <config.h>

IniFile::IniFile(const char* path) : _path(path){};
    
    /* open the file for each call and search for the parameter */
String IniFile::getValue(const char* key) {
    String result = "";
    
    File f = SPIFFS.open(_path, "r");
    if (!f) {
        DEBUG("file open failed");
        return "";
    }
    
    // create key indicator
    String key_id(key);
    key_id += "=";
    
    while(f.available())
    {
        String line = f.readStringUntil('\n');
        // check if we hit the line with the key;
        if(line.startsWith(key_id))
        {
            result = line.substring(key_id.length());
            break;
        }
    }
    
    f.close();
    
    // remove cartage return
    result.replace("\r","");
    return result;
}

/*
 Function to begin the update process
 */
bool IniFile::beginUpdate()
{
    if(exists())
    {
        SPIFFS.remove(_path);
    }
    
    fd = SPIFFS.open(_path, "w");
    
    return (bool) fd;
}

void IniFile::endUpdate()
{
    fd.close();
}

void IniFile::updateValue(const char* key, const char* val)
{
    
    String line(key);
    line += "=";
    line +=  val;
    
    DEBUG("appending line: " + line);
    fd.println(line);
}

void IniFile::clearAll()
{
    SPIFFS.remove(_path);
}