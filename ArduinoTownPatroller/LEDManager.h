#include "arduino.h"

struct LEDInfo
{
    int PinNumber;
    bool IsRunning;
};

class LEDManager
{
public : 
    LEDInfo *LEDs;

    LEDManager()
    {
        
    }

    LEDManager(int num)
    {
        LEDs = new LEDInfo[num];
    }

    void SetLEDInfo(int pinnum, int index)
    {
        LEDs[index].PinNumber = pinnum;
        LEDs[index].IsRunning = false;
    }
    
    void LED_Write(int pinindex, bool status)
    {
        if(status == true)
        {
            analogWrite(LEDs[pinindex].PinNumber, 255);
            LEDs[pinindex].IsRunning = true;
        }
        else
        {
            analogWrite(LEDs[pinindex].PinNumber, 0);
            LEDs[pinindex].IsRunning = false;
        }
    }
};