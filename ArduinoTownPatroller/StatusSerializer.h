#include "arduino.h"

class StatusSerializer
{
public :
    StatusSerializer()
    {
        
    }

    void SerializeSinglePacket(char type, int value, char *target)
    {
        String result = String();
        result.concat("{");
        result.concat(String(type));
        result.concat(String(value));
        result.concat("}");

        Serial.print(result.length());
        for(int i = 0; i < result.length(); i++)
        {
            target[i] = result[i];
        }
        target[result.length()] = '\0';
    }
};