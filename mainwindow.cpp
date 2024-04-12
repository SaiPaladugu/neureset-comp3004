#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), currentSelection(NewSession), currentDisplay(Menu), timer(new QTimer(this))
{
    ui->setupUi(this);

    neureset = new Neureset();

    // set default label background to clear and other elements to invisible
    ui->new_session->setStyleSheet("background-color: #FFFFFF");
    ui->session_log->setStyleSheet("background-color: #FFFFFF");
    ui->time_date->setStyleSheet("background-color: #FFFFFF");
    ui->timer->setVisible(false);
    ui->session_progress->setVisible(false);
    ui->session_log_data->setVisible(false);

    // hide date and time initially but the values
    ui->dateTimeEdit->setVisible(false);
    currentDateTime = QDateTime::currentDateTime();
    ui->dateTimeEdit->setDateTime(currentDateTime);

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
    currentDisplay = option;
    updateDisplay(option);
}

void MainWindow::startSession(){
    if(neureset->isRunning()) {
        neureset->unpauseSession();
        timer->start(1000);  // continue the countdown
    } else {
        // if not already running, start new timer
        QtConcurrent::run(std::mem_fn(&Neureset::newSession), neureset);
        int seconds = 7 + (1 + neureset->incrementTimer) * 7;
        int minutes = seconds / 60;
        int remainingSeconds = seconds % 60;
        QTime startTime(0, minutes, remainingSeconds);
        ui->timer->setTime(startTime);
        timer->start(1000);
    }
}


void MainWindow::pauseSession()
{
    neureset->pauseSession();
    timer->stop();
}

void MainWindow::stopSession(){
    neureset->stopSession();
    QTime time(0, 0, 0);
    ui->timer->setTime(time);
    timer->stop();
    changeDisplay(Menu);
}

void MainWindow::updateTimer() {
    QTime currentTime = ui->timer->time();
    int seconds = QTime(0, 0, 0).secsTo(currentTime);
    if (seconds > 1) {
        currentTime = currentTime.addSecs(-1);
        ui->timer->setTime(currentTime);
    } else {
        timer->stop();
        stopSession();
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

void MainWindow::updateDisplay(MenuOption option)
{
    if (option == NewSession) {
        startNeuresetSession();
        // timer and progress bar as specs indicate (+ wtv else we want)
        ui->new_session->setVisible(false);
        ui->session_log->setVisible(false);
        ui->time_date->setVisible(false);
        ui->timer->setVisible(true);
        ui->session_progress->setVisible(true);
        ui->dateTimeEdit->setVisible(false);
        ui->session_log_data->setVisible(false);
        ui->start->setEnabled(true);
        ui->stop->setEnabled(true);
        ui->pause->setEnabled(true);
    } else if (option == SessionLog) {
        updateSessionLogDisplay();
        // uhh not sure yet smtn to do w the DB file
        ui->new_session->setVisible(false);
        ui->session_log->setVisible(false);
        ui->time_date->setVisible(false);
        ui->timer->setVisible(false);
        ui->session_progress->setVisible(false);
        ui->dateTimeEdit->setVisible(false);
        ui->session_log_data->setVisible(true);
        ui->start->setEnabled(false);
        ui->stop->setEnabled(false);
        ui->pause->setEnabled(false);
    } else if (option == TimeDate) {
        updateDateTimeDisplay();
        ui->new_session->setVisible(false);
        ui->session_log->setVisible(false);
        ui->time_date->setVisible(false);
        ui->timer->setVisible(false);
        ui->session_progress->setVisible(false);
        ui->dateTimeEdit->setVisible(true);
        ui->session_log_data->setVisible(false);
        ui->start->setEnabled(false);
        ui->stop->setEnabled(false);
        ui->pause->setEnabled(false);
    } else {
        // default menu display (same as constructor)
        // set default label background to clear and other elements to invisible
        ui->new_session->setVisible(true);
        ui->session_log->setVisible(true);
        ui->time_date->setVisible(true);
        ui->session_log->setStyleSheet("background-color: #FFFFFF");
        ui->time_date->setStyleSheet("background-color: #FFFFFF");
        ui->timer->setVisible(false);
        ui->session_progress->setVisible(false);
        ui->dateTimeEdit->setVisible(false);
        ui->session_log_data->setVisible(false);
        ui->start->setEnabled(false);
        ui->stop->setEnabled(false);
        ui->pause->setEnabled(false);

        // setting first option, new_session, to be chosen and yellow as default
        ui->new_session->setStyleSheet("background-color: #FFFF00");
        currentSelection = NewSession;
    }
}

void MainWindow::updateDateTimeDisplay()
{
    ui->dateTimeEdit->setDateTime(currentDateTime);
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
    int seconds = 7 + (neureset->incrementTimer + 1) * 7;
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    QTime time(0, minutes, remainingSeconds);
    ui->timer->setTime(time);
}

























