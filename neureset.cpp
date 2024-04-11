#include "neureset.h"

Neureset::Neureset(QObject *parent) : beeping(false), time(QDateTime::currentDateTime()) {
    qInfo() << "Neureset created";
    // Create the eeg sites
    for (int i = 0 ; i < NUM_SITES; i++){
        sites[i] = new EEGSite();
    }

    initialAverageBaseline = -1;

    for (int i = 0 ; i < NUM_LIGHTS/3; i = i+3){
        lights[i] = new Light("blue");
        lights[i+1] = new Light("green");
        lights[i+2] = new Light("red");
    }
    therapyTimer = new QTimer(this);
    connect(therapyTimer, &QTimer::timeout, this, &Neureset::processNextSite);
}

Neureset::~Neureset() {
    qInfo() << "Neureset destroyed";
    for (int i = 0; i < NUM_SITES; i++){
        delete sites[i];
    }
    for (int i = 0 ; i < NUM_LIGHTS/3; i = i+3){
        delete lights[i];
        delete lights[i+1];
        delete lights[i+2];
    }
    delete therapyTimer;;
}

void Neureset::newSession(){
    if (therapyTimer->isActive()){
        therapyTimer->stop();
    }
    calculateBaseline();

    Session* session = new Session();
    session->startBaseline = this->initialAverageBaseline;
    session->dateTime = QDateTime::currentDateTime();
    sessions.push_back(session);


    //therapyTimer->start(1000);
    currentSiteIndex = 0;
    while(currentSiteIndex < NUM_SITES){
        processNextSite();
    }
    finishSession();
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
    for (int i = 0; i < NUM_SITES; i++){
        finalAverageBaseline += sites[i]->getBaseline();
    }
    finalAverageBaseline /= NUM_SITES;

    // Set the endBaseline of the current session
    if (!sessions.empty()){
        sessions.back()->endBaseline = finalAverageBaseline;
    }

    qInfo() << "End baseline" << finalAverageBaseline;

    // Stop the therapy timer
    if (therapyTimer->isActive()){
        therapyTimer->stop();
    }
}

void Neureset::processNextSite(){
        // Process the current site
        sites[currentSiteIndex]->calculateSiteBaseline();
        sites[currentSiteIndex]->applyTreatment();
        currentSiteIndex++;
}

void Neureset::changeDateTime(QDateTime newTime){
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
    QFuture<void> futures[NUM_SITES];
    for (int i = 0; i < NUM_SITES; i++){
        futures[i] = QtConcurrent::run(sites[i], &EEGSite::calculateSiteBaseline);
    }

    // Wait for all calculations to finish
    for (int i = 0; i < NUM_SITES; i++){
        futures[i].waitForFinished();
    }

    // Calculate average baseline across EEG
    initialAverageBaseline = 0;
    for (int i = 0; i < NUM_SITES; i++){
        initialAverageBaseline += sites[i]->getBaseline();
    }
    initialAverageBaseline /= NUM_SITES;

    qInfo()  << "average initial" << initialAverageBaseline;
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
