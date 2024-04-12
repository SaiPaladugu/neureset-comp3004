#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QtConcurrent/QtConcurrent>

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
    connect(ui->pause, &QPushButton::clicked, this, &MainWindow::pauseSession);
    connect(ui->start, &QPushButton::clicked, this, &MainWindow::startSession);
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

void MainWindow::pauseSession()
{
    neureset.pauseSession();
}

void MainWindow::startSession(){
    if(neureset.isRunning()) neureset.unpauseSession();
    else QtConcurrent::run(std::mem_fn(&Neureset::newSession), &neureset);
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
























