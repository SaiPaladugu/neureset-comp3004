#include "eegsite.h"

EEGSite::EEGSite()
{
    siteBaseline = 0;
    incrementTimer = 1;
    band = QRandomGenerator::global()->bounded(1,6);

    generateFrequencies();
}
EEGSite::~EEGSite(){}

void EEGSite::generateFrequencies(){
    for(int i = 0; i < 3; i++){
       //Randomly generate amplitudes
        amplitudes[i] = QRandomGenerator::global()->bounded(0,101);

       //Randomly generate frequencies for each bands
        switch(band){
            //Alpha band
            case 1:
                frequencies[i] = QRandomGenerator::global()->bounded(8,13);
                break;
            //Beta band
            case 2:
                frequencies[i] = QRandomGenerator::global()->bounded(12,31);
                break;
            //Delta band
            case 3:
                frequencies[i] = QRandomGenerator::global()->bounded(1,5);
                break;
            //Theta band
            case 4:
                frequencies[i] = QRandomGenerator::global()->bounded(4,9);
                break;
            //Gamma band
            case 5:
                frequencies[i] = QRandomGenerator::global()->bounded(25,141);
                break;
            default:
                qInfo() << "Error calculating baseline";
               frequencies[i] = 0;
            break;
        }
    }
}

void EEGSite::applyTreatment(){
    for(int i = 0; i < 16; i++){
        siteBaseline = siteBaseline + 5;
        QThread::msleep(63);
    }
}

int EEGSite::getBaseline(){
    return siteBaseline;
}

void EEGSite::setBaseline(int baseline){
    siteBaseline = baseline;
}

int* EEGSite::getFrequencies(){
    return frequencies;
}

int* EEGSite::getAmplitudes(){
    return amplitudes;
}

int EEGSite::getBand(){
    return band;
}
