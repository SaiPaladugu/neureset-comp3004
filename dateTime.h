#ifndef DATETIME_H
#define DATETIME_H

#include <QDateTime>
#include <QDebug>


class dateTime
{
public:
    dateTime();
    ~dateTime();
    void getDateTime();
private:
    int year;
    int month;
    int day;
    char* dt;

};

#endif // DATETIME_H
