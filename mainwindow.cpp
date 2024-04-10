#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dateTime.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    date.getDateTime();
}

MainWindow::~MainWindow()
{
    delete ui;
}

