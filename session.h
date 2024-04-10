#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QDateTime>

class Session {
    public:
        Session();
        ~Session();
        int startBaseline;
        int endBaseline;
        int progress;
        QDateTime dateTime;

};

#endif
