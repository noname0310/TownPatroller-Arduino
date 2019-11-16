#include <SoftwareSerial.h>
#include <ArduinoSTL.h>
#include "Sonarmanager.h"
#include "MotorManager.h"
#include "StatusDeserializer.h"
#include "LEDManager.h"

#define SystemToggle 9

#define SONARFront 12
#define SONARLeftH 11
#define SONARRightH 10
#define SONARLeftS 9
#define SONARRightS 8

#define FrontDist sonarManager.distance[0]
#define RightDist sonarManager.distance[3]
#define LeftDist sonarManager.distance[4]

#define MAX_DISTANCE 1000 // maximum distance for sensors


#define BreakLight 13

#define DIRB 7
#define PWMB 6
#define PWMA 5
#define DIRA 4

#define BTRx 3
#define BTTx 2

using namespace std;

SonarManager sonarManager;
MotorManager motorManager;
LEDManager ledManager;
StatusDeserializer statusDeserializer;
SoftwareSerial BluetoothSerial(BTTx, BTRx);
char SingleChar = '\0';

bool ReadMode;

void setup() 
{
    Serial.begin(9600);
    BluetoothSerial.begin(9600);

    initSystemToggle();
    initSonars();
    initMotors();
    initLEDs();
    initParser();

    ReadMode = true;
}

void loop() 
{
    if(ReadMode)
    {
        BluetoothIO();
    }
    else
    {
        sonarManager.updateSonar();
        BluetoothSerial.write('[');
        SendSonarInfo();
        SendMotorInfo();
        SendLEDInfo();
        BluetoothSerial.write(']');

        ReadMode = true;
    }
}

void BluetoothIO()
{
    if (BluetoothSerial.available()) 
    {
        SingleChar = (char)BluetoothSerial.read();

        if(SingleChar == ']')
            ReadMode = false;

        BTPacket bTPacket = statusDeserializer.AddDeserializeQueue(SingleChar);

        if(bTPacket.type != '\0' && bTPacket.value <= 255)
        {
            switch (bTPacket.type)
            {
            case 'f':
                motorManager.MotorPWMWrite(1, bTPacket.value);
                break;
            case 'g':
                motorManager.MotorPWMWrite(0, bTPacket.value);
                break;
            case 'h':
                motorManager.MotorDIRWrite(1, bTPacket.value);
                break;
            case 'i':
                motorManager.MotorDIRWrite(0, bTPacket.value);
                break;
            case 'j':
                if(bTPacket.value == 1)
                    ledManager.LED_Write(0, true);
                else
                    ledManager.LED_Write(0, false);
                break;
            case 'k':
                if(bTPacket.value == 1)
                    ledManager.LED_Write(1, true);
                else
                    ledManager.LED_Write(1, false);
                break;
            case 'l':
                if(bTPacket.value == 1)
                    ledManager.LED_Write(2, true);
                else
                    ledManager.LED_Write(2, false);
                break;
            case 'm':
                if(bTPacket.value == 1)
                    ledManager.LED_Write(3, true);
                else
                    ledManager.LED_Write(3, false);
                break;
            default:
                break;
            }
        }
    }
}

void SendSonarInfo()
{
    char type = 'a';

    for (int i = 0; i < sonarManager.distance.size(); i++)
    {
        SendPacket(type, sonarManager.distance[i]);
        type++;
    }
}

void SendMotorInfo()
{
    SendPacket('f', motorManager.Motors[2].Power);
    SendPacket('g', motorManager.Motors[1].Power);
    SendPacket('h', motorManager.Motors[3].Power);
    SendPacket('i', motorManager.Motors[0].Power);
}

void SendLEDInfo()
{
    SendPacket('j', ledManager.LEDs[0].IsRunning);
    SendPacket('k', ledManager.LEDs[1].IsRunning);
    SendPacket('l', ledManager.LEDs[2].IsRunning);
    SendPacket('m', ledManager.LEDs[3].IsRunning);
}

void SendPacket(char type, int value)
{
    char cstr[16];
    itoa(value, cstr, 10);

    BluetoothSerial.write('{');
    BluetoothSerial.write(type);
    BluetoothSerial.write(cstr);
    BluetoothSerial.write('}');
}

void AutoWallDrive()
{
    if(digitalRead(SystemToggle) == 0)
    {
        motorManager.Stop();
    }
    else
    {
        sonarManager.updateSonar();
        
        if(RightDist <= 10)
        {
            motorManager.LRotation(100);
        }
        else if(RightDist >= 30 && RightDist < 80)
        {
            motorManager.RRotation(100);
        }
        else if(FrontDist >= 10)
        {
            motorManager.Forward(255);
        }
        else
        {
            motorManager.LRotation(255);
            delay(1000);
        }
    }
}

void initSystemToggle()
{
    pinMode(SystemToggle, INPUT);
}

void initSonars()
{
    vector<SonarInfo> sonarinfo;
    
    sonarinfo.push_back(SonarInfo("SONARFront", NewPing(SONARFront, SONARFront, MAX_DISTANCE)));
    sonarinfo.push_back(SonarInfo("SONARRightH", NewPing(SONARRightH, SONARRightH, MAX_DISTANCE)));
    sonarinfo.push_back(SonarInfo("SONARLeftH", NewPing(SONARLeftH, SONARLeftH, MAX_DISTANCE)));
    sonarinfo.push_back(SonarInfo("SONARRightS", NewPing(SONARRightS, SONARRightS, MAX_DISTANCE)));
    sonarinfo.push_back(SonarInfo("SONARLeftS", NewPing(SONARLeftS, SONARLeftS, MAX_DISTANCE)));

    sonarManager = SonarManager(sonarinfo, MAX_DISTANCE);
}

void initMotors()
{
    pinMode(DIRA, OUTPUT);

    digitalWrite(DIRA, LOW);
    analogWrite(PWMA, 0);


    pinMode(DIRB, OUTPUT);

    digitalWrite(DIRB, LOW);
    analogWrite(PWMB, 0); 

    pinMode(BreakLight, OUTPUT);

    MotorInfo *motorinfo = new MotorInfo[4]
    {
        {"DIRA", DIRA, false},
        {"PWMA", PWMA, false},
        {"PWMB", PWMB, false},
        {"DIRB", DIRB, false}
    };

    motorManager = MotorManager(motorinfo, BreakLight);
}

void initLEDs()
{
    ledManager = LEDManager(4);

    ledManager.SetLEDInfo(A0, 0);
    ledManager.SetLEDInfo(A1, 1);
    ledManager.SetLEDInfo(A2, 2);
    ledManager.SetLEDInfo(A3, 3);
}

void initParser()
{
    statusDeserializer = StatusDeserializer();
}