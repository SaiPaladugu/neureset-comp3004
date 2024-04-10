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
#include "dateTime.h"

class Session;
class EEGsite;

class Neureset : public QObject {
    Q_OBJECT
private:
    QVector<Session*> sessions;
    QVector<EEGsite*> sites;
    QDateTime time;
    bool beeping;

    // new stuff to be added to UML
    float intialAverageBaseline;
    QTimer* therapyTimer;
    QTimer* pauseTimer;
    bool sessionInProgress;
    bool sessionPaused;
    int currentSiteIndex;
    int totalSites = 7;
    

public:
    Neureset(QObject *parent = nullptr);
    ~Neureset();

    void newSession();
    void pauseSession();
    void unpauseSession();
    void finishSession();
    // void stopSession();
    void changeDateTime(QDateTime newTime);
    QVector<Session*>& sessionLog();
    void beep();
    void calculateBaseline();
    void notify(QString message);

    bool exportSessionData(const QString& filepath, const QVector<Session*>& sessions);
    QVector<Session*> importSessionData(const QString& filename);

public slots;
    void stopSession();
    void processNextSite();
}

#endif // NERUSET_H