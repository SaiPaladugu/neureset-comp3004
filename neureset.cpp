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
    paused = true;

    sessions = importSessionData("sessions_data.txt");
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
}

void Neureset::newSession(){
    running = true;
    if (paused == true) paused = false;
    calculateBaseline();

    Session* session = new Session();
    session->startBaseline = this->initialAverageBaseline;
    session->dateTime = QDateTime::currentDateTime();
    sessions.push_back(session);


    //therapyTimer->start(1000);
    currentSiteIndex = 0;
    while(currentSiteIndex < NUM_SITES && !paused){
        processNextSite();
    }
    if (currentSiteIndex == NUM_SITES) finishSession();
}

void Neureset::pauseSession(){
    paused = true;
}

void Neureset::unpauseSession(){
    paused = false;

    while(currentSiteIndex < NUM_SITES && !paused){
        processNextSite();
    }
    if (currentSiteIndex == NUM_SITES) finishSession();

}

void Neureset::stopSession(){
    paused = true;
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
    running = false;
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


bool Neureset::isRunning(){
    return running;
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

    QString relativeFilePath = SOURCE_DIR + '/' + filename;
    qDebug()<<relativeFilePath;
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
