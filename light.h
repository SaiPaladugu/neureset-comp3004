#ifndef LIGHT_H
#define LIGHT_H

#include <QString>

class Light
{
public:
    Light(QString);
    ~Light();
    void changeLight(QString);
    QString getColour();
    QString getIllumination();
private:
    QString colour;
    QString illumination;
};

#endif // LIGHT_H
