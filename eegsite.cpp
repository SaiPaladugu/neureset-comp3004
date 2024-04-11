#include "eegsite.h"

EEGSite::EEGSite()
{
    siteBaseline = 0;
}
EEGSite::~EEGSite(){}

int EEGSite::calculateSiteBaseline(){
    qDebug()<<"Calculating baseline";
    int band = QRandomGenerator::global()->bounded(1,6);
    int frequencies[3];
    int amplitudes[3];

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
    QThread::sleep(3);
    siteBaseline = (frequencies[0] * amplitudes[0] + frequencies[1] * amplitudes[1] + frequencies[2] * amplitudes[2])/(amplitudes[0] + amplitudes[1] + amplitudes[2]);
    return siteBaseline;
}

void EEGSite::applyTreatment(){
    for(int i = 0; i < 16; i++){
        siteBaseline = siteBaseline + 5;
        QThread::msleep(63);
    }
    //Confirmation message to be moved to neureset class using .notify
    //qInfo() << "Treatment applied";
}

int EEGSite::getBaseline(){
    return siteBaseline;
}
