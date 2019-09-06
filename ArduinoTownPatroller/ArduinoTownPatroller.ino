#include <SoftwareSerial.h>
#include <ArduinoSTL.h>
#include "Sonarmanager.h"
#include "MotorManager.h"
#include "StatusDeserializer.h"
#include "StatusSerializer.h"

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
StatusDeserializer statusDeserializer;
StatusSerializer statusSerializer;
SoftwareSerial BluetoothSerial(BTTx, BTRx);
char SingleChar = '\0';
int SonarUpdateCount = 0;

void setup() 
{
    Serial.begin(9600);
    BluetoothSerial.begin(9600);

    initSystemToggle();
    initSonars();
    initMotors();
    initParser();
}

void loop() 
{
    if(SonarUpdateCount == 1000)
    {
        sonarManager.updateSonar();
        SendSonarInfo();
        SonarUpdateCount = 0;
    }
    else
    {
        SonarUpdateCount++;
    }
    BluetoothIO();
}

void BluetoothIO()
{
    if (Serial.available())
    {         
        BluetoothSerial.write(Serial.read());
    }

    if (BluetoothSerial.available()) 
    {
        SingleChar = (char)BluetoothSerial.read();

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

void SendPacket(char type, int value)
{
    char cstr[16];
    itoa(value, cstr, 10);
    //char chmsg[50] = "sdasdasdsadsa";
    //statusSerializer.SerializeSinglePacket(type, value, chmsg);

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

void initParser()
{
    statusDeserializer = StatusDeserializer();
    statusSerializer = StatusSerializer();
}