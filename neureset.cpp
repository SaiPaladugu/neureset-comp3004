#include "neureset.h"

Neureset::Neureset() : time(QDateTime::currentDateTime()) {
    qInfo() << "Neureset created";

    //Create the eeg sites
    for (int i = 0 ; i < NUM_SITES; i++){
        sites.append(new EEGSite());
    }

    //Calculation variables
    currentSiteIndex = 0;
    initialAverageBaseline = -1;
    incrementTimer = 1;

    //Lights
    lights[0] = new Light("blue");
    lights[1] = new Light("green");
    lights[2] = new Light("red");

    //Status
    beeping = false;
    paused = true;
    running = false;
    flash = false;

    //Session
    curSession = nullptr;
    QVector<Session*> importedSessions = importSessionData("sessions_data.txt");
    for (Session* session : importedSessions){
        sessions.append(session);
    }

    //Time
    time = QDateTime::currentDateTime();
    pauseTimer = new QTimer(this);
    connect(pauseTimer, &QTimer::timeout, this, &Neureset::stopSession);
}

Neureset::~Neureset() {
    qInfo() << "Neureset destroyed";

    //Deleting sites
    for (int i = 0; i < NUM_SITES; i++){
        delete sites.at(i);
    }

    //Deleting lights
    delete lights[0];
    delete lights[1];
    delete lights[2];

    //Deleting timer
    delete pauseTimer;
}

void Neureset::newSession(){
    if (isRunning()) return;

    running = true;
    if (paused) paused = false;

    //Start the session
    lights[0]->changeLight("ON");
    emit lightChanged(0);
    calculateBaseline();
    currentSiteIndex = 0;

    curSession = new Session();
    curSession->startBaseline = this->initialAverageBaseline;
    curSession->dateTime = QDateTime::currentDateTime();

    siteProcessing();
}

void Neureset::pauseSession(){
    if(!running) return;

    paused = true;

    //Reset pause shutdown timer if active
    if (pauseTimer->isActive()) pauseTimer->stop();

    //Start pause shutdown timer
    pauseTimer->start(300000);
}

void Neureset::unpauseSession(){
    if(!running) return;

    if(paused){
            paused = false;
            pauseTimer->stop();
        }

    //Resume calculations asynchronously
     QtConcurrent::run(std::mem_fn(&Neureset::siteProcessing), this);
}

void Neureset::siteProcessing(){
    while(currentSiteIndex < NUM_SITES && !paused){
        processNextSite();
    }
    if (currentSiteIndex == NUM_SITES) {
      finishSession();
    }
}

void Neureset::stopSession(){
    if(isRunning() == false) return;
    qDebug()<<"Stopping session, most recent session will not be saved";

    paused = true;
    running = false;
    currentSiteIndex = 0;

    if(pauseTimer->isActive()) pauseTimer->stop();

    if (curSession != nullptr){
        delete curSession;
        curSession = nullptr;
    }

    //Emit signal to stop mainwindow timer
    emit stop();
}

void Neureset::finishSession(){
    // Calculate average final baseline across EEG
    int finalAverageBaseline = 0;
    for (int i = 0; i < NUM_SITES; i++){
        finalAverageBaseline += sites[i]->getBaseline();
    }
    finalAverageBaseline /= NUM_SITES;

    // Set the endBaseline of the current session
    sessions.append(curSession);
    if (!sessions.empty()){
        sessions.back()->endBaseline = finalAverageBaseline;
    }
    qInfo() << "End baseline" << finalAverageBaseline;

    running = false;
    paused = false;
    currentSiteIndex = 0;
    curSession = nullptr;

    qDebug()<<"Session finished and saved.";

    //Emit signal to stop mainwindow timer
    emit stop();
}

bool Neureset::calibrateSite(){
    bool valid = true;
    int band = sites.at(currentSiteIndex)->getBand();

    for(int i = 0; i < 3; i++){
        //Check amplitude
        if(sites.at(currentSiteIndex)->getAmplitudes()[i] > 100 || sites.at(currentSiteIndex)->getAmplitudes()[i] < 0) {
            valid = false;
            break;
        }
        //Chec band freuency
        switch(band){
            //Alpha band
            case 1:
                if(sites.at(currentSiteIndex)->getFrequencies()[i] > 12 || sites.at(currentSiteIndex)->getFrequencies()[i] < 8){
                    valid = false;
                }
            break;
            //Beta band
            case 2:
                if(sites.at(currentSiteIndex)->getFrequencies()[i] > 30 || sites.at(currentSiteIndex)->getFrequencies()[i] < 12){
                    valid = false;
                }
            break;
            //Delta band
            case 3:
                if(sites.at(currentSiteIndex)->getFrequencies()[i] > 4 || sites.at(currentSiteIndex)->getFrequencies()[i] < 1){
                    valid = false;
                }
            break;;
            //Theta band
            case 4:
                if(sites.at(currentSiteIndex)->getFrequencies()[i] > 8 || sites.at(currentSiteIndex)->getFrequencies()[i] < 4){
                    valid = false;
                }
            break;
            //Gamma band
            case 5:
                if(sites.at(currentSiteIndex)->getFrequencies()[i] > 140 || sites.at(currentSiteIndex)->getFrequencies()[i] < 25){
                    valid = false;
                }
            break;
            //Unknown band
            default:
                qInfo() << "Unknown band";
                valid = false;
            break;
        }
    }
    return valid;
}

void Neureset::processNextSite(){
        // Process the current site

        notify("Calculating site baseline");
        sites.at(currentSiteIndex)->generateFrequencies();

        if(calibrateSite() == true){
            int* frequencies = sites.at(currentSiteIndex)->getFrequencies();
            int* amplitudes = sites.at(currentSiteIndex)->getAmplitudes();

            int calculation = (frequencies[0] * amplitudes[0] * amplitudes[0]  + frequencies[1] * amplitudes[1] * amplitudes[1] + frequencies[2] *
                    amplitudes[2] * amplitudes[2])/(amplitudes[0] * amplitudes[0] + amplitudes[1] * amplitudes[1] + amplitudes[2] * amplitudes[2]);
            sites.at(currentSiteIndex)->setBaseline(calculation);
        }
        else{
            //Data is invalid
            stopSession();
        }
        QThread::msleep(950);

        if(!paused){
            //Flash green light on
            lights[1]->changeLight("ON");
            emit lightChanged(1);

            sites.at(currentSiteIndex)->applyTreatment();
            QThread::msleep(1000);

            //Flash green light off
            lights[1]->changeLight("OFF");
            emit lightChanged(4);
            notify("Treatment applied");

            currentSiteIndex++;
        }
}

void Neureset::changeDateTime(QDateTime newTime){
    // TODO: Figure what this is for, maybe take in a dateTime object
    time = newTime;
}

void Neureset::beepFlash(){
    if (beeping && paused) {
        QTimer::singleShot(1000, this, &Neureset::beepFlash);
        if (!flash && running){
            //Beep and flash on
            qInfo() << "Beep!";
            flash = true;
            emit lightChanged(2);
        }
        else{
            //Flash off
            flash = false;
            emit lightChanged(5);
        }
    }
}

void Neureset::calculateBaseline(){
    // Calculate baselines
    for (int i = 0; i < NUM_SITES; i++){
        if(calibrateSite() == true){
            int* frequencies = sites.at(i)->getFrequencies();
            int* amplitudes = sites.at(i)->getAmplitudes();

            int calculation = (frequencies[0] * amplitudes[0] * amplitudes[0]  + frequencies[1] * amplitudes[1] * amplitudes[1] + frequencies[2] *
                    amplitudes[2] * amplitudes[2])/(amplitudes[0] * amplitudes[0] + amplitudes[1] * amplitudes[1] + amplitudes[2] * amplitudes[2]);
            sites.at(i)->setBaseline(calculation);
        }
        else{
            //Data invalid
            stopSession();
        }
    }
    QThread::msleep(950);

    // Calculate average baseline across EEG
    initialAverageBaseline = 0;
    for (int i = 0; i < NUM_SITES; i++){
        initialAverageBaseline += sites[i]->getBaseline();
    }
    initialAverageBaseline /= NUM_SITES;

    qInfo()  << "average initial" << initialAverageBaseline;
}

void Neureset::contactLostProtocol(){
    notify("Contact lost");
    emit pause();
    lights[2]->changeLight("FLASHING");
    lights[0]->changeLight("OFF");
    emit lightChanged(3);
    beeping = true;
    beepFlash();

    //Demo purposes assume user reconnects
    QTimer::singleShot(10000, this, &Neureset::contactReestablishedProtocol);
}

void Neureset::contactReestablishedProtocol(){
    beeping = false;
    notify("Contact Reestablished");
    lights[0]->changeLight("ON");
    lights[2]->changeLight("OFF");
    emit lightChanged(0);
    emit unpause();
}

bool Neureset::exportSessionData(const QString& filename, const QVector<Session*>& sessions){
    qDebug()<<"In export";
    qDebug() << "Current working directory:" << QDir::currentPath();
    QString sourceDir = SOURCE_DIR;
    QString relativeFilePath = sourceDir + '/' + filename;
    qDebug()<<relativeFilePath;

    QFile::remove(relativeFilePath);
    QFile file(relativeFilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning() << "Could not open file for writing";
        return false;
    }

    QTextStream out(&file);
    for (const Session* session : sessions){
        if (session){
            qDebug()<<"Exporting"<<session->startBaseline;
            out << session->startBaseline <<",";
            out << session->endBaseline <<",";
            out << session->progress <<",";
            out << session->dateTime.toString("yyyy-MM-dd hh:mm:ss") <<"\n";
        }
    }

    file.close();
    return true;
}

QVector<Session*> Neureset::importSessionData(const QString& filename){
    QVector<Session*> sessions;

    QString sourceDir = SOURCE_DIR;
    QString relativeFilePath = sourceDir + '/' + filename;
    qDebug()<<"Fetching data from: "<< relativeFilePath;
    QFile file(relativeFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "Could not open file for reading";
        return sessions;
    }

    QTextStream in(&file);
    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() == 4){
            Session* session = new Session();
            session->startBaseline = fields.at(0).toInt();
            session->endBaseline = fields.at(1).toInt();
            session->progress = fields.at(2).toInt();
            session->dateTime = QDateTime::fromString(fields.at(3), "yyyy-MM-dd hh:mm:ss");
            sessions.push_back(session);
        }
    }

    file.close();
    return sessions;
}

void Neureset::notify(QString message){qInfo() << message;}

bool Neureset::isRunning() {return running;}

QVector<Session*>& Neureset::sessionLog() {return sessions;}

bool Neureset::isPaused(){return paused;}

int Neureset::getIncrement(){return  incrementTimer;}

QVector<EEGSite*> Neureset::getSites(){ return sites; }

int Neureset::getCurrentSiteIndex(){ return currentSiteIndex; }
