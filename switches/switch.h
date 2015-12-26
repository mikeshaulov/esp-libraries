//
//  switch.h
//  
//
//  Created by michael shaulov on 11/2/15.
//
//

#ifndef switch_h
#define switch_h

#include <Arduino.h>


// Base switch class that provides TurnOn and TurnOff methods
class CBaseSwitch
{
private:
    int m_pin;
    int m_onMode;
    int m_offMode;
    bool m_state;
    
public:
    CBaseSwitch(int pin, int onMode, int offMode) : m_pin(pin), m_onMode(onMode), m_offMode(offMode)
    {
        pinMode(m_pin, OUTPUT);
        TurnOff();
        m_state = false;
    }
    
    void TurnOn()
    {
        digitalWrite(m_pin, m_onMode);
        m_state = true;
    }
    
    void TurnOff()
    {
        digitalWrite(m_pin, m_offMode);
        m_state = false;
    }
    
    void TurnBool(bool bOn)
    {
        if(bOn){
            TurnOn();
        }
        else {
            TurnOff();
        }
    }
    
    bool GetState() const
    {
        return m_state;
    }
    
    void Blink(int milli) {TurnOn(); delay(milli); TurnOff();}
};

// switch that sets LOW when turned on and HIGH when turned off
class CReverseSwitch : public CBaseSwitch
{
public:
    CReverseSwitch(int pin) : CBaseSwitch(pin, LOW, HIGH){}
};


// simple led switch
class CLed : public CBaseSwitch
{
public:
    CLed(int pin) : CBaseSwitch(pin, HIGH, LOW){}
};


#endif /* switch_h */
