#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QDateTime>

class Session {
    public:
        Session();
        ~Session();
        float startBaseline;
        float endBaseline;
        int progress;
        QDateTime dateTime;

};

#endif
