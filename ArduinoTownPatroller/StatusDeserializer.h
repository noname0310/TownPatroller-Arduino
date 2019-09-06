#include "arduino.h"

struct BTPacket
{
    int value;
    char type;
};

class StatusDeserializer
{
private :
    int index = 0;
    char packettype = '\0';
    String packetvalue = "";

    int getCarry(int index)
    {
        if(index == 0)
            return 1;
        else
        {
            int result = 1;

            for(int i = 0; i < index; i++)
            {
                result *= 10;
            }

            return result;
        }
    }

    void IntParse(String msg, int *value)
    {
        *value = 0;

        for (int i = 0; i < packetvalue.length(); i++) 
        {
            *value += (packetvalue[packetvalue.length() - 1 - i] - '0') * getCarry(i);
        }
    }

public :
    BTPacket AddDeserializeQueue(char singlechar)
    {
        switch (index)
        {
            case 0:
                if (singlechar != '{')
                {
                    index = -1;
                    packetvalue = "";
                }

                break;
            case 1:
                if ('a' <= singlechar && singlechar <= 'z')
                    packettype = singlechar;
                else
                {
                    index = -1;
                    packetvalue = "";
                }

                break;
            default:
                if ('0' <= singlechar && singlechar <= '9')
                    packetvalue.concat(singlechar);
                else
                {
                    if(singlechar == '}')
                    {
                        int value = 0;
                        IntParse(packetvalue, &value);

                        index = 0;
                        packetvalue = "";
                        BTPacket bp;
                        bp.type = packettype;
                        bp.value = value;
                        return bp;
                    }

                    index = -1;
                    packetvalue = "";
                }

                break;
        }
        
        index++;

        return BTPacket{0, '\0'};
    }
};