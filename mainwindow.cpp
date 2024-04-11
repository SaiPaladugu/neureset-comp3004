#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QThread>

#include <neureset.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), currentSelection(NewSession), currentDisplay(Menu)
{
    ui->setupUi(this);


    // set default label background to clear and other elements to invisible
    ui->new_session->setStyleSheet("background-color: #FFFFFF");
    ui->session_log->setStyleSheet("background-color: #FFFFFF");
    ui->time_date->setStyleSheet("background-color: #FFFFFF");
    ui->timer->setVisible(false);
    ui->session_progress->setVisible(false);

    // setting first option, new_session, to be highlighted yellow
    ui->new_session->setStyleSheet("background-color: #FFFF00");

    // handling button selection
    connect(ui->up_arrow, &QPushButton::clicked, this, &MainWindow::onUpArrowPressed);
    connect(ui->down_arrow, &QPushButton::clicked, this, &MainWindow::onDownArrowPressed);
    connect(ui->select, &QPushButton::clicked, this, &MainWindow::onSelectPressed);
    connect(ui->menu, &QPushButton::clicked, this, [=]() {
        this->changeDisplay(MainWindow::Menu);
    });

    // Mock sessions creation
//    Neureset neureset;
//    QVector<Session*> mockSessions;
//    for (int i = 0; i < 5; ++i) {
//        Session* session = new Session();
//        session->startBaseline = i * 10; // Sample data
//        session->endBaseline = i * 20;
//        session->progress = 100;
//        session->dateTime = QDateTime::currentDateTime().addDays(-i);
//        mockSessions.append(session);
//    }

//    // Export mock sessions to a file
//    QString sourceDirPath = SOURCE_DIR;
//    QString filepath = sourceDirPath + "/sessions_data.txt";
//    if (neureset.exportSessionData(filepath, mockSessions)) {
//        qDebug() << "Sessions exported successfully.";
//    } else {
//        qDebug() << "Failed to export sessions.";
//    }

//    // Cleanup
//    qDeleteAll(mockSessions);
//    mockSessions.clear();

//    // Call the import method
//    QVector<Session*> importedSessions = neureset.importSessionData(filepath);

//    // Verify the imported sessions
//    qDebug() << "Imported Sessions:";
//    for (const Session* session : importedSessions) {
//        qDebug() << "Start Baseline:" << session->startBaseline
//                 << ", End Baseline:" << session->endBaseline
//                 << ", Progress:" << session->progress
//                 << ", DateTime:" << session->dateTime.toString("yyyy-MM-dd hh:mm:ss");
//    }

//    // Remember to clean up the imported sessions to avoid memory leaks
//    qDeleteAll(importedSessions);

    Neureset neureset;

    // Start a new session and observe the process
    qDebug() << "Starting a new session...";
    neureset.newSession();

//    QThread::sleep(600);

    // Optionally, test export functionality
//    QString filepath = "sessions_data.txt";
//    if (neureset.exportSessionData(filepath, neureset.sessionLog())) {
//        qDebug() << "Session data exported successfully to:" << filepath;
//    } else {
//        qDebug() << "Failed to export session data.";
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
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
        return; // no highlighting in other menus
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

void MainWindow::updateDisplay(MenuOption option)
{
    if (option == NewSession) {
        // timer and progress bar as specs indicate (+ wtv else we want)
        ui->new_session->setVisible(false);
        ui->session_log->setVisible(false);
        ui->time_date->setVisible(false);
        ui->timer->setVisible(true);
        ui->session_progress->setVisible(true);
    } else if (option == SessionLog) {
        // uhh not sure yet smtn to do w the DB file
        ui->new_session->setVisible(false);
        ui->session_log->setVisible(false);
        ui->time_date->setVisible(false);
        ui->timer->setVisible(false);
        ui->session_progress->setVisible(false);
    } else if (option == TimeDate) {
        // also not super sure
        // is this just straight up just time n date??
        ui->new_session->setVisible(false);
        ui->session_log->setVisible(false);
        ui->time_date->setVisible(false);
        ui->timer->setVisible(true);
        ui->session_progress->setVisible(false);
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

        // setting first option, new_session, to be chosen and yellow as default
        ui->new_session->setStyleSheet("background-color: #FFFF00");
        currentSelection = NewSession;
    }

    updateDateTimeDisplay(); // no clue what to do here ngl
}

void MainWindow::updateDateTimeDisplay()
{
    // logic
}
























