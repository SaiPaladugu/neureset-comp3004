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
    //Data
    QVector<Session*> sessions;
    QVector<EEGSite*> sites;
    QDateTime time;
    Light* lights[NUM_LIGHTS];
    QTimer* pauseTimer;

    //Calculation
    int initialAverageBaseline;
    int currentSiteIndex;
    int incrementTimer;

    //Status
    bool paused;
    bool running;
    bool flash;
    bool beeping;

    Session* curSession;
    

public:
    Neureset();
    ~Neureset();

    //Functionality
    virtual void newSession();
    virtual void pauseSession();
    virtual void unpauseSession();
    virtual void finishSession();
    virtual void stopSession();
    virtual void calculateBaseline();
    void notify(QString message);

    //Menu
    void changeDateTime(QDateTime newTime);
    virtual QVector<Session*>& sessionLog();

    //Import + Export
    virtual bool exportSessionData(const QString& filepath, const QVector<Session*>& sessions);
    virtual QVector<Session*> importSessionData(const QString& filename);

    //Status
    bool isRunning();
    bool isPaused();

    //Calculation
    void processNextSite();
    void siteProcessing();
    bool calibrateSite();
    int getIncrement();

    //Safety
    void contactLostProtocol();
    void contactReestablishedProtocol();
    void beepFlash();

signals:
    void lightChanged(int light);
    void pause();
    void unpause();
    void stop();
};

#endif // NERUSET_H
