#ifndef EEGSITE_H
#define EEGSITE_H

#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

class EEGSite {
public:
    EEGSite();
    ~EEGSite();
    void generateFrequencies();
    void applyTreatment();
    int getBaseline();
    void setBaseline(int);
    int* getFrequencies();
    int* getAmplitudes();
    int getBand();

private:
    int siteBaseline;
    int frequencies[3];
    int amplitudes[3];
    int band;
};

#endif
