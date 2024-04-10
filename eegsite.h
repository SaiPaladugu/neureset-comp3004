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

private:
    int siteBaseline;
};

#endif
