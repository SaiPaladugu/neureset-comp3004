#ifndef EEGSITE_H
#define EEGSITE_H

#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

class eegSite : public QObject
{
public:
    eegSite();
    ~eegSite();
    int calculateSiteBaseline();
    void applyTreatment();
    int getBaseline();

private:
    int siteBaseline;
};

#endif // DATETIME_H
