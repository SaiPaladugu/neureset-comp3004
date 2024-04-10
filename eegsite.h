#ifndef EEGSITE_H
#define EEGSITE_H

#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

class EEGSite {
    Q_OBJECT
public:
    EEGSite();
    ~EEGSite();
    int calculateSiteBaseline();
    void applyTreatment();
    int getBaseline();

private:
    int siteBaseline;
};

#endif // DATETIME_H
