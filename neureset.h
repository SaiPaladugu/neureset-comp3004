#ifndef NERUSET_H
#define NERUSET_H


#include <QString>
#include <QVector>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include "eegsite.h"
#include "session.h"
#include "mediator.h"
#include "light.h"

#define NUM_SITES 7
#define NUM_LIGHTS 3

class Session;
class EEGSite;

class Neureset : public QObject, public Mediator {
    Q_OBJECT
private:
    QVector<Session*> sessions;
    EEGSite* sites[NUM_SITES];
    QDateTime time;
    bool beeping;
    Light* lights[NUM_LIGHTS];

    // new stuff to be added to UML
    int initialAverageBaseline;
    QTimer* therapyTimer;
    QTimer* pauseTimer;
    bool sessionInProgress;
    bool sessionPaused;
    int currentSiteIndex;
    

public:
    Neureset(QObject *parent = nullptr);
    ~Neureset();

    virtual void newSession();
    virtual void pauseSession();
    virtual void unpauseSession();
    virtual void finishSession();
    // void stopSession();
    void changeDateTime(QDateTime newTime);
    virtual QVector<Session*>& sessionLog();
    void beep();
    virtual void calculateBaseline();
    void notify(QString message);

    virtual bool exportSessionData(const QString& filepath, const QVector<Session*>& sessions);
    virtual QVector<Session*> importSessionData(const QString& filename);

public slots:
    void stopSession();
    void processNextSite();
};

#endif // NERUSET_H
