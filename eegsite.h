#ifndef EEGSITE_H
#define EEGSITE_H

#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

class EEGSite {
public:
    EEGSite();
    ~EEGSite();
    int calculateSiteBaseline();
    void applyTreatment();
    int getBaseline();
    int incrementTimer;

private:
    int siteBaseline;
};

#endif
