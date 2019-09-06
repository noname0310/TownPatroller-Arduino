#include <ArduinoSTL.h>
#include "arduino.h"
#include <NewPing.h>

using namespace std;

class SonarInfo
{
public : 
    String sonarname;
    NewPing ping;

    SonarInfo(String SonarName, NewPing Ping) : ping(0, 0, 0)
    {
        sonarname = SonarName;
        ping = Ping;
    }
};

class SonarManager
{
private :
    int MAX_DISTANCE;
    vector<SonarInfo> sonar;

public : 
    vector<int> distance;

    SonarManager()
    {
        MAX_DISTANCE = 1000;
    }

    SonarManager(vector<SonarInfo> sonarinfo, int max_distance)
    {
        //copy(sonarinfo, sonar);
        sonar = sonarinfo;

        distance = vector<int>(sonarinfo.size());
        MAX_DISTANCE = max_distance;
    }

    void updateSonar()
    {
        for (int i = 0; i < sonar.size(); i++) 
        {
            distance[i] = sonar[i].ping.ping_cm();

            if (distance[i] == 0)
                distance[i] = MAX_DISTANCE;
        }
    }
    void printSonarsValue()
    {
        for (int i = 0; i < sonar.size(); i++) 
        {
            Serial.print(sonar[i].sonarname + ": ");
            Serial.print(distance[i]);
            Serial.print("\n");
        }
    }
};