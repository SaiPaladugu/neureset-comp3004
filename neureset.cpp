#include "neureset.h"

Neureset::Neureset(QObject *parent) : beeping(false), time(QDateTime::currentDateTime()) {
    qInfo() << "Neureset created";
    // Create the eeg sites
    for (int i = 0 ; i < NUM_SITES; i++){
        sites.append(new EEGSite());
    }

    currentSiteIndex = 0;
    initialAverageBaseline = -1;

    lights[0] = new Light("blue");
    lights[1] = new Light("green");
    lights[2] = new Light("red");

    beeping = false;
    paused = true;

    QVector<Session*> importedSessions = importSessionData("sessions_data.txt");
    for (Session* session : importedSessions){
        sessions.append(session);
    }

    pauseTimer = new QTimer(this);
    curSession = nullptr;
}

Neureset::~Neureset() {
    qInfo() << "Neureset destroyed";
    for (int i = 0; i < NUM_SITES; i++){
        delete sites.at(i);
    }
    // Deleting lights
    delete lights[0];
    delete lights[1];
    delete lights[2];
}

void Neureset::newSession(){
    if (isRunning()){
        return;
    }
    running = true;
    if (paused == true) paused = false;
    lights[0]->changeLight("ON");
    emit lightChanged();
    calculateBaseline();

    curSession = new Session();
    curSession->startBaseline = this->initialAverageBaseline;
    curSession->dateTime = QDateTime::currentDateTime();

    currentSiteIndex = 0;
    siteProcessing();
}

void Neureset::pauseSession(){
    if(!running) return;

    paused = true;

    if (pauseTimer->isActive()) {
        pauseTimer->stop();
    }

    connect(pauseTimer, &QTimer::timeout, this, &Neureset::stopSession);
    pauseTimer->start(2000);
}

void Neureset::unpauseSession(){
    if(!running) return;

    if(paused){
            paused = false;
            pauseTimer->stop();
        }

    siteProcessing();
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
    qDebug()<<"Stopping session, most recent session will not be saved";
    paused = true;
    running = false;
    if(pauseTimer->isActive()) pauseTimer->stop();

    if (curSession != nullptr){
        delete curSession;
        curSession = nullptr;
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
    sessions.append(curSession);
    if (!sessions.empty()){
        sessions.back()->endBaseline = finalAverageBaseline;
    }

    qInfo() << "End baseline" << finalAverageBaseline;
    running = false;
    paused = false;
    curSession = nullptr;
    qDebug()<<"Session finished and saved.";
}

void Neureset::processNextSite(){
        // Process the current site
        notify("Calculating site baseline");
        sites[currentSiteIndex]->calculateSiteBaseline();
        lights[2]->changeLight("ON");
        emit lightChanged();
        sites[currentSiteIndex]->applyTreatment();
        notify("Treatment applied");
        lights[2]->changeLight("OFF");
        emit lightChanged();
        sites.at(currentSiteIndex)->calculateSiteBaseline();
        sites.at(currentSiteIndex)->applyTreatment();
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


bool Neureset::isPaused(){
    return paused;
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
