#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QVector>
#include "session.h"


class Mediator
{
public:
    Mediator();
    virtual ~Mediator();
    virtual void newSession() = 0;
    virtual void pauseSession() = 0;
    virtual void unpauseSession() = 0;
    virtual void finishSession() = 0;
    virtual QVector<Session*>& sessionLog() = 0;
    virtual void calculateBaseline() = 0;
    virtual bool exportSessionData(const QString&, const QVector<Session*>&) = 0;
    virtual QVector<Session*> importSessionData(const QString&) = 0;
};

#endif // MEDIATOR_H
