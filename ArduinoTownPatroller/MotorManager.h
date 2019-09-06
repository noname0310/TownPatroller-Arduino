#include "arduino.h"

#define IDIRA 0
#define IPWMA 1
#define IPWMB 2
#define IDIRB 3

struct MotorInfo
{
    String MotorName;
    int PinNumber;
    bool IsRunning;
};

class MotorManager
{
private:
    int BreakLight;

    void MotorWrite(int pinindex, uint8_t status)
    {
        if(status == HIGH)
        {
            digitalWrite(Motors[pinindex].PinNumber, HIGH);
            Motors[pinindex].IsRunning = true;
        }
        else
        {
            digitalWrite(Motors[pinindex].PinNumber, LOW);
            Motors[pinindex].IsRunning = false;
        }
    }

    void MotorAnalogWrite(int pinindex, int power)
    {
        if(power != 0)
        {
            analogWrite(Motors[pinindex].PinNumber, power);
            Motors[pinindex].IsRunning = true;
        }
        else
        {
            analogWrite(Motors[pinindex].PinNumber, power);
            Motors[pinindex].IsRunning = false;
        }
    }

    void UpdateBreakLight()
    {
        if(Motors[IDIRA].IsRunning == false && Motors[IDIRB].IsRunning == false)
        {
            digitalWrite(BreakLight, HIGH);
        }
        else
        {
            digitalWrite(BreakLight, LOW);
        }
        
    }

public : 
    MotorInfo *Motors;

    MotorManager()
    {
    }

    MotorManager(MotorInfo *motorinfo, int BreakPin)
    {
        Motors = motorinfo;
        BreakLight = BreakPin;
    }

    ~MotorManager()
    {
        //delete Motors;
    }

    void MotorDIRWrite(int LR, uint8_t status)
    {
        switch (LR)
        {
        case 0:
            MotorWrite(IDIRB, status);
            break;
        case 1:
            MotorWrite(IDIRA, status);
            break;
        default:
            break;
        }
    }

    void MotorPWMWrite(int LR, int power)// 0 = L     1 = R
    {
        switch (LR)
        {
        case 0:
            MotorAnalogWrite(IPWMB, power);
            break;
        case 1:
            MotorAnalogWrite(IPWMA, power);
            break;
        default:
            break;
        }
    }

    void Forward_LRSetP(int Lpower, int Rpower)
    {
        MotorWrite(IDIRA, HIGH);  
        MotorAnalogWrite(IPWMA, Rpower);
        MotorWrite(IDIRB, HIGH);  
        MotorAnalogWrite(IPWMB, Lpower);

        UpdateBreakLight();
    }

    void Forward(int Power)
    {
        MotorWrite(IDIRA, HIGH);  
        MotorAnalogWrite(IPWMA, Power);
        MotorWrite(IDIRB, HIGH);  
        MotorAnalogWrite(IPWMB, Power);

        UpdateBreakLight();
    }

    void Backward(int Power)
    {
        MotorWrite(IDIRA, LOW);  
        MotorAnalogWrite(IPWMA, Power);
        MotorWrite(IDIRB, LOW);  
        MotorAnalogWrite(IPWMB, Power);

        UpdateBreakLight();
    }

    void Stop()
    {
        MotorWrite(IDIRA, LOW);  
        MotorAnalogWrite(IPWMA, 0);
        MotorWrite(IDIRB, LOW);  
        MotorAnalogWrite(IPWMB, 0);

        UpdateBreakLight();
    }

    void RRotation(int Power)
    {
        MotorWrite(IDIRA, HIGH);  
        MotorAnalogWrite(IPWMA, Power);
        MotorWrite(IDIRB, LOW);  
        MotorAnalogWrite(IPWMB, Power);

        UpdateBreakLight();
    }

    void LRotation(int Power)
    {
        MotorWrite(IDIRA, LOW);  
        MotorAnalogWrite(IPWMA, Power);
        MotorWrite(IDIRB, HIGH);  
        MotorAnalogWrite(IPWMB, Power);

        UpdateBreakLight();
    }
};