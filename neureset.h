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

#define NUM_SITES 7

class Session;
class EEGSite;

class Neureset : public QObject {
    Q_OBJECT
private:
    QVector<Session*> sessions;
    QVector<EEGSite*> sites;
    QDateTime time;
    bool beeping;

    // new stuff to be added to UML
    int intialAverageBaseline;
    QTimer* therapyTimer;
    QTimer* pauseTimer;
    bool sessionInProgress;
    bool sessionPaused;
    int currentSiteIndex;
    int battery;
    

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

    int getBattery() const;
    bool charge();

public slots:
    void stopSession();
    void processNextSite();
};

#endif // NERUSET_H
