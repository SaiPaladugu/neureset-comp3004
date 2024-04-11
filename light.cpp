#include "light.h"

Light::Light(QString color)
{
   colour = color;
}

Light::~Light(){}

void Light::changeLight(QString lighting){
    illumination = lighting;
}

QString Light::getColour(){
    return colour;
}

QString Light::getIllumination(){
    return illumination;
}
