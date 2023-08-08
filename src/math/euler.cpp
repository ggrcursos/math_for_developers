#include "euler.h"
#include<cmath>
#include "vector.h"
    Vector EAngle::ToVector() const
    {
        Vector result;
        result.x = cos(yaw) * cos(pitch);
        result.y = sin(pitch);
        result.z = sin(yaw)*cos(pitch);
        return result;
    }
    void EAngle:: Normalize()
    {
        if (pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
        while( yaw < -180.0f) yaw += 360;
        while( yaw > 180) yaw -= 360;
    }