#include "dateTime.h"

dateTime::dateTime()
{

}
dateTime::~dateTime(){}

void dateTime::getDateTime(){
    QDateTime now = QDateTime::currentDateTime();
    QString english = now.toString("yyyy-MM-dd hh:mm:ss");
    qInfo() << english;
}
