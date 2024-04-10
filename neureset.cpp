#include "neureset.h"

Neureset::Neureset() : beeping(false), time(QDateTime::currentDateTime()) {
    qInfo() << "Neureset created";
    // Create the eeg sites
    for (int i = 0 ; i < 7; i++){
        sites.append(new EEGsite());
    }

    intialAverageBaseline = -1;

}

Neureset::~Neureset() {
    qInfo() << "Neureset destroyed";
    for (int i = 0; i < sites.size(); i++){
        delete sites.at(i);
    }
    sites.clear();
}

void Neureset::newSession(){
    if (therapyTimer){
        therapyTimer->stop();
        delete therapyTimer;
    }
    
    calculateBaseline();

    Session* session = new Session();
    session->startBaseline = this->intialAverageBaseline;
    session->dateTime = QDateTime::currentDateTime();
    sessions.push_back(session);


    therapyTimer = new QTimer(this);
    connect(therapyTimer, &QTimer::timeout, this, &Neureset::processNextSite);
    therapyTimer->start(1000);
    currentSite = 0;
}

void Neureset::pauseSession(){
    if(therapyTimer){
        therapyTimer->stop();
    }

     // Start a timer that will stop the session after 5 minute
    pauseTimer = new QTimer(this);
    connect(pauseTimer, &QTimer::timeout, this, &Neureset::stopSession);
    pauseTimer->start(300000); 
}

void Neureset::unpauseSession(){
    if(therapyTimer){
        therapyTimer->start(1000);
    }

    // If the pause timer is running, stop it
    if (pauseTimer){
        pauseTimer->stop();
        delete pauseTimer;
        pauseTimer = nullptr;
    }
}

void Neureset::stopSession(){
 if (therapyTimer){
        therapyTimer->stop();
        delete therapyTimer;
        therapyTimer = nullptr;
    }
    if (!sessions.empty()){
        delete sessions.back();
        sessions.pop_back();
    }
}

void Neureset::finishSession(){
    // Calculate average final baseline across EEG
    int finalAverageBaseline = 0;
    for (int i = 0; i < sites.size(); i++){
        finalAverageBaseline += sites.at(i)->siteBaseline;
    }
    finalAverageBaseline /= sites.size();

    // Set the endBaseline of the current session
    if (!sessions.empty()){
        sessions.back()->endBaseline = finalAverageBaseline;
    }

    // Stop the therapy timer
    if (therapyTimer){
        therapyTimer->stop();
        delete therapyTimer;
        therapyTimer = nullptr;
    }
}

void Neureset::processNextSite(){
    if (currentSite < sites.size()){
        // Process the current site
        sites.at(currentSite)->calculateSiteBaseline();
        sites.at(currentSite)->applyTreatment();
        currentSite++;
    } else {
        finishSession();
    }
}

void Neuroset::changeDateTime(QDateTime newTime){
    // TODO: Figure what this is for, maybe take in a dateTime object
    time = newTime;
}

QVector<Session*>& Neureset::sessionLog(){
    return sessions;
}

void Neureset::beep(){
    // TODO: Figure what this is for, maybe below is a implementation for it
    if (!beeping){
        beeping = true;
        qInfo() << "Beep!";
        QTimer::singleShot(1000, this, &Neureset::beep);
    } else {
        beeping = false;
    }
}

void Neureset::calculateBaseline(){
    // Calculate baselines
    QFuture<void> futures[7];
    for (int i = 0; i < sites.size(); i++){
        futures[i] = QtConcurrent::run(sites.at(i), &EEGsite::calculateBaseline);
    }

    // Wait for all calculations to finish
    for (int i = 0; i < sites.size(); i++){
        futures[i].waitForFinished();
    }

    // Calculate average baseline across EEG
    intialAverageBaseline = 0;
    for (int i = 0; i < sites.size(); i++){
        intialAverageBaseline += sites.at(i)->siteBaseline;
    }
    intialAverageBaseline /= sites.size();
}

void Neureset::notify(QString message){
    qDebug() << message;
}




bool Neureset::exportSessionData(const QString& filepath, const QVector<Session*>& sessions){
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning() << "Could not open file for writing";
        return false;
    }

    QTextStream out(&file);
    for (const Session* session : sessions){
        if (session){
            out << session->startBaseline <<",";
            out << session->endBaseline <<",";
            out << session->progress <<",";
            out << session->dateTime.toString("yyyy-MM-dd hh:mm:ss") <<"\n";
        }
    }

    file.close();
    return true;
}

QVector<Session*> Neureset::importSessionData(const QString& filepath){
    QVector<Session*> sessions;
    QFile file(filepath);
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
