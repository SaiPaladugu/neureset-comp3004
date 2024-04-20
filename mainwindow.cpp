#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), currentSelection(NewSession), currentDisplay(Menu), timer(new QTimer(this))
{
    powerStatus = true;
    ui->setupUi(this);

    neureset = new Neureset();

    // set default label background to clear and other elements to invisible
    ui->new_session->setStyleSheet("background-color: #FFFFFF");
    ui->session_log->setStyleSheet("background-color: #FFFFFF");
    ui->time_date->setStyleSheet("background-color: #FFFFFF");
    ui->timer->setVisible(false);
    ui->session_progress->setVisible(false);
    ui->session_log_data->setVisible(false);
    ui->graph->setVisible(false);

    // hide date and time initially but set the values up
    ui->dateTimeEdit->setVisible(false);
    currentDateTime = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(currentDateTime);
    // updateGraphData();

    // setting first option, new_session, to be highlighted yellow
    ui->new_session->setStyleSheet("background-color: #FFFF00");

    // handling display related button selection
    connect(ui->up_arrow, &QPushButton::clicked, this, &MainWindow::onUpArrowPressed);
    connect(ui->down_arrow, &QPushButton::clicked, this, &MainWindow::onDownArrowPressed);
    connect(ui->select, &QPushButton::clicked, this, &MainWindow::onSelectPressed);
    connect(ui->menu, &QPushButton::clicked, this, [=]() {
        this->changeDisplay(MainWindow::Menu);
    });

    // handling neureset related buttons
    connect(ui->start, &QPushButton::clicked, this, &MainWindow::startSession);
    connect(ui->pause, &QPushButton::clicked, this, &MainWindow::pauseSession);
    connect(ui->stop, &QPushButton::clicked, this, &MainWindow::stopSession);
    connect(ui->dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &MainWindow::updateDateTimeDisplay);
    connect(neureset, &Neureset::lightChanged, this, &MainWindow::lightChange);

    // disable start/play/pause unless in new session
    ui->start->setEnabled(false);
    ui->stop->setEnabled(false);
    ui->pause->setEnabled(false);

    // new session timer
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    connect(neureset, &Neureset::stop, this, &MainWindow::stop);

    //Contact lost button
    connect(ui->contact_lost, &QPushButton::clicked, this, &MainWindow::contactLost);
    connect(neureset, &Neureset::pause, this, &MainWindow::pauseSession);
    connect(neureset, &Neureset::unpause, this, &MainWindow::startSession);

    // power
    connect(ui->power, &QPushButton::clicked, this, &MainWindow::onPowerButtonClicked);

    // Export
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::exportData);

    // safety
    ui->contact_lost->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete neureset;
    delete timer;
}

void MainWindow::onUpArrowPressed()
{
    if (currentSelection == NewSession) {
        return; // No action
    }
    currentSelection = static_cast<MenuOption>(currentSelection - 1);
    highlightCurrentSelection();
}

void MainWindow::onDownArrowPressed()
{
    if (currentSelection == TimeDate) {
        return; // No action
    }
    currentSelection = static_cast<MenuOption>(currentSelection + 1);
    highlightCurrentSelection();
}

void MainWindow::onSelectPressed()
{
    if (currentDisplay != Menu) {
        return; // implement the use of select in other menus later :3
    }
    changeDisplay(currentSelection);
}

void MainWindow::highlightCurrentSelection()
{
    if (currentDisplay != Menu) {
        return; // no highlighting in other menus (currently)
    }

    ui->new_session->setStyleSheet("background-color: #FFFFFF");
    ui->session_log->setStyleSheet("background-color: #FFFFFF");
    ui->time_date->setStyleSheet("background-color: #FFFFFF");

    if (currentSelection == SessionLog) {
        ui->session_log->setStyleSheet("background-color: #FFFF00");
    } else if (currentSelection == TimeDate) {
        ui->time_date->setStyleSheet("background-color: #FFFF00");
    } else if (currentSelection == NewSession) {
        ui->new_session->setStyleSheet("background-color: #FFFF00");
    }

}

void MainWindow::changeDisplay(MenuOption option)
{
    if(neureset->isRunning() && currentDisplay == NewSession) {
        stopSession();
    }
    currentDisplay = option;
    updateDisplay(option);
}

void MainWindow::startSession(){
    if(neureset->isRunning()) {
        qDebug() << "unpausing";
        timer->start(1000);  // continue the countdown
        neureset->unpauseSession();
    } else { // create new session
        // if not already running, start new timer
        QtConcurrent::run(std::mem_fn(&Neureset::newSession), neureset);
        int seconds = 1 + (1 + neureset->getIncrement()) * 7;
        int minutes = seconds / 60;
        int remainingSeconds = seconds % 60;
        QTime startTime(0, minutes, remainingSeconds);
        ui->timer->setTime(startTime);
        timer->start(1000);
    }
}


void MainWindow::pauseSession()
{
    qDebug() << "pausing";
    neureset->pauseSession();
    timer->stop();
}

void MainWindow::stopSession(){
    neureset->stopSession();
    QTime time(0, 0, 0);
    ui->timer->setTime(time);
    timer->stop();
    changeDisplay(Menu);
    ui->session_progress->setValue(0);
    ui->graph->clearGraphs();
    ui->graph->replot();
}

void MainWindow::updateTimer() {
    updateGraphData(neureset->getCurrentSiteIndex());
    QTime currentTime = ui->timer->time();
    int currentSeconds = QTime(0, 0, 0).secsTo(currentTime);
    int seconds = QTime(0, 0, 0).secsTo(currentTime);
    if (seconds > 1) {
        currentTime = currentTime.addSecs(-1);
        ui->timer->setTime(currentTime);
        int elapsedSeconds = totalDurationInSeconds - currentSeconds;
        ui->session_progress->setValue(elapsedSeconds);
    } else {
        timer->stop();
        stopSession();
        ui->graph->clearGraphs();
        ui->graph->replot();

        int newBatteryLevel = ui->battery->value() - 50;
        if (newBatteryLevel < 0) newBatteryLevel = 0;
        ui->battery->setValue(newBatteryLevel);
        if (newBatteryLevel == 0) {
            powerStatus = true;
            onPowerButtonClicked();
            disableAll();
            disableSafety();
            ui->power->setEnabled(true);
            ui->menu->setEnabled(false);
            ui->up_arrow->setEnabled(false);
            ui->down_arrow->setEnabled(false);
            ui->select->setEnabled(false);
            ui->exportButton->setEnabled(false);
        }
    }
}

void MainWindow::lightChange(int light){
    //Change light in display Sai
    light++;
    switch(light){
        case 1:
            ui->light_1->setStyleSheet("background-color: blue");
            break;
        case 2:
            ui->light_2->setStyleSheet("background-color: green");
            break;
        case 3:
            ui->light_3->setStyleSheet("background-color: red");
            break;
        case 4:
            ui->light_1->setStyleSheet("background-color: white");
            break;
        case 5:
            ui->light_2->setStyleSheet("background-color: white");
            break;
        case 6:
            ui->light_3->setStyleSheet("background-color: white");
            break;
        default:
            ui->light_1->setStyleSheet("background-color: white");
            ui->light_2->setStyleSheet("background-color: white");
            ui->light_3->setStyleSheet("background-color: white");
    }
}

void MainWindow::resetLight() {
    ui->light_1->setStyleSheet("background-color: #FFFFFF");
    ui->light_2->setStyleSheet("background-color: #FFFFFF");
    ui->light_3->setStyleSheet("background-color: #FFFFFF");
}

void MainWindow::contactLost(){
    neureset->contactLostProtocol();
}

void MainWindow::stop(){
    changeDisplay(Menu);
}

void MainWindow::updateDisplay(MenuOption option)
{
    resetLight();
    disableAll();
    if (option == NewSession) {
        startNeuresetSession();
        // timer and progress bar as specs indicate (+ wtv else we want)
        ui->timer->setVisible(true);
        ui->session_progress->setVisible(true);
        ui->start->setEnabled(true);
        ui->stop->setEnabled(true);
        ui->pause->setEnabled(true);
        ui->contact_lost->setEnabled(true);
        ui->graph->setVisible(true);
    } else if (option == SessionLog) {
        updateSessionLogDisplay();
        ui->session_log_data->setVisible(true);
    } else if (option == TimeDate) {
        updateDateTimeDisplay();
        ui->dateTimeEdit->setVisible(true);
    } else {
        // default menu display (same as constructor)
        // set default label background to clear and other elements to invisible
        ui->up_arrow->setEnabled(true);
        ui->down_arrow->setEnabled(true);
        ui->select->setEnabled(true);
        ui->menu->setEnabled(true);
        ui->new_session->setVisible(true);
        ui->session_log->setVisible(true);
        ui->time_date->setVisible(true);
        ui->session_log->setStyleSheet("background-color: #FFFFFF");
        ui->time_date->setStyleSheet("background-color: #FFFFFF");

        // setting first option, new_session, to be chosen and yellow as default
        ui->new_session->setStyleSheet("background-color: #FFFF00");
        currentSelection = NewSession;
    }
}

void MainWindow::updateDateTimeDisplay()
{
    currentDateTime = ui->dateTimeEdit->dateTime();
}

void MainWindow::updateSessionLogDisplay()
{
    // clear existing data and fetch the new data
    ui->session_log_data->clear();
    QVector<Session*>& sessions = neureset->sessionLog();

    // appending logic
    for (Session* session : sessions) {
        if (session) {
            QString sessionInfo = QString("Start Baseline: %1, End Baseline: %2, Date: %3")
                                  .arg(session->startBaseline)
                                  .arg(session->endBaseline)
                                  .arg(session->dateTime.toString("yyyy-MM-dd hh:mm:ss"));
            ui->session_log_data->addItem(sessionInfo);
        }
    }
}

void MainWindow::startNeuresetSession()
{
    int seconds = 1 + (neureset->getIncrement() + 1) * 7;
    totalDurationInSeconds = seconds;
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    QTime time(0, minutes, remainingSeconds);
    ui->timer->setTime(time);

    ui->session_progress->setMaximum(totalDurationInSeconds);
    ui->session_progress->setValue(0);
}


void MainWindow::updateGraphData(int siteIndex) {
    if (siteIndex > 7 || siteIndex < 0) {
        return;
    }
    QVector<double> x(101), y(101);

    double f1 = neureset->getSites().at(siteIndex)->getFrequencies()[0];
    double A1 = neureset->getSites().at(siteIndex)->getAmplitudes()[0];
    double f2 = neureset->getSites().at(siteIndex)->getFrequencies()[1];
    double A2 = neureset->getSites().at(siteIndex)->getAmplitudes()[1];
    double f3 = neureset->getSites().at(siteIndex)->getFrequencies()[2];
    double A3 = neureset->getSites().at(siteIndex)->getAmplitudes()[2];

    double fd = (f1 * A1 * A1 + f2 * A2 * A2 + f3 * A3 * A3) / (A1 * A1 + A2 * A2 + A3 * A3);

    double xShift = ((rand() % 1000) / 1000.0) * 2 * M_PI;

    for (int i = 0; i < 101; ++i) {
        x[i] = i / 10.0 - 10;
        y[i] = sin(fd * x[i] + xShift);
    }

    ui->graph->xAxis->setTickLabels(false);
    ui->graph->yAxis->setTickLabels(false);
    ui->graph->addGraph();
    ui->graph->graph(0)->setData(x, y);
    ui->graph->rescaleAxes();
    ui->graph->replot();
}


void MainWindow::disableSafety() {
    ui->contact_lost->setEnabled(false);
}

void MainWindow::disableMenuButtons() {
    ui->up_arrow->setEnabled(false);
    ui->down_arrow->setEnabled(false);
    ui->select->setEnabled(false);
    ui->menu->setEnabled(false);
}

void MainWindow::disableAll() {
    ui->new_session->setVisible(false);
    ui->session_log->setVisible(false);
    ui->time_date->setVisible(false);
    ui->timer->setVisible(false);
    ui->session_progress->setVisible(false);
    ui->dateTimeEdit->setVisible(false);
    ui->session_log_data->setVisible(false);
    ui->start->setEnabled(false);
    ui->stop->setEnabled(false);
    ui->pause->setEnabled(false);
    ui->contact_lost->setEnabled(false);
    ui->graph->setVisible(false);
}

void MainWindow::onPowerButtonClicked() {
    // if dead, follow dead battery protocol
    if (ui->battery->value() == 0) {
        QMessageBox::critical(this, "Battery Status", "Battery dead");
        return;
    }

    // otherwise, handle like normal
    if (powerStatus == true) {
        disableAll();
        disableMenuButtons();
        powerStatus = false;
    } else if (powerStatus == false) {
        changeDisplay(Menu);
        powerStatus = true;
    }
}

void MainWindow::exportData(){
    QString filename = QString("sessions_data.txt");
    QtConcurrent::run(std::mem_fn(&Neureset::exportSessionData), neureset, filename);
}



















