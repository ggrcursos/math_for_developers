#ifndef EULER_H
#define EULER_H

#include<string>


class Vector;
class EAngle
{
    public:
    EAngle()
    {
        pitch = yaw = roll = 0.0f;
    }

    std::string toString()
    {
        return "pitch: "+ std::to_string(pitch) + 
        "  yaw: "+ std::to_string(yaw) + 
        "  roll: "+ std::to_string(roll) ;
    }

    EAngle(float _pitch, float _yaw, float _roll)
    {   
        pitch = _pitch;
        yaw = _yaw;
        roll = _roll;
    }

    Vector ToVector() const;
    void Normalize();

    public:
    float pitch;
    float yaw;
    float roll;

};
#endif