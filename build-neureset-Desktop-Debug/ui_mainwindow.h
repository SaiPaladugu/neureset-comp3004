/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QProgressBar *battery;
    QPushButton *contact;
    QPushButton *treatment;
    QPushButton *contact_lost;
    QPushButton *power;
    QGroupBox *display;
    QProgressBar *session_progress;
    QLabel *new_session;
    QLabel *session_log;
    QLabel *time_date;
    QLabel *timer;
    QPushButton *menu;
    QPushButton *up_arrow;
    QPushButton *select;
    QPushButton *down_arrow;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(796, 455);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        battery = new QProgressBar(centralwidget);
        battery->setObjectName(QString::fromUtf8("battery"));
        battery->setGeometry(QRect(650, 400, 118, 23));
        battery->setValue(100);
        contact = new QPushButton(centralwidget);
        contact->setObjectName(QString::fromUtf8("contact"));
        contact->setGeometry(QRect(10, 10, 91, 25));
        treatment = new QPushButton(centralwidget);
        treatment->setObjectName(QString::fromUtf8("treatment"));
        treatment->setGeometry(QRect(10, 50, 91, 25));
        contact_lost = new QPushButton(centralwidget);
        contact_lost->setObjectName(QString::fromUtf8("contact_lost"));
        contact_lost->setGeometry(QRect(10, 90, 91, 25));
        power = new QPushButton(centralwidget);
        power->setObjectName(QString::fromUtf8("power"));
        power->setEnabled(true);
        power->setGeometry(QRect(700, 10, 83, 25));
        display = new QGroupBox(centralwidget);
        display->setObjectName(QString::fromUtf8("display"));
        display->setGeometry(QRect(250, 120, 281, 151));
        session_progress = new QProgressBar(display);
        session_progress->setObjectName(QString::fromUtf8("session_progress"));
        session_progress->setGeometry(QRect(90, 120, 118, 23));
        session_progress->setValue(100);
        new_session = new QLabel(display);
        new_session->setObjectName(QString::fromUtf8("new_session"));
        new_session->setGeometry(QRect(10, 30, 111, 17));
        new_session->setStyleSheet(QString::fromUtf8("background-color: white"));
        session_log = new QLabel(display);
        session_log->setObjectName(QString::fromUtf8("session_log"));
        session_log->setGeometry(QRect(10, 60, 111, 17));
        session_log->setStyleSheet(QString::fromUtf8("background-color: white"));
        time_date = new QLabel(display);
        time_date->setObjectName(QString::fromUtf8("time_date"));
        time_date->setGeometry(QRect(10, 90, 111, 17));
        time_date->setStyleSheet(QString::fromUtf8("background-color: white"));
        timer = new QLabel(display);
        timer->setObjectName(QString::fromUtf8("timer"));
        timer->setGeometry(QRect(120, 50, 61, 41));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(18);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        timer->setFont(font);
        timer->setLayoutDirection(Qt::LeftToRight);
        timer->setAutoFillBackground(false);
        timer->setStyleSheet(QString::fromUtf8("background-color: white"));
        menu = new QPushButton(centralwidget);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu->setGeometry(QRect(250, 280, 83, 25));
        up_arrow = new QPushButton(centralwidget);
        up_arrow->setObjectName(QString::fromUtf8("up_arrow"));
        up_arrow->setGeometry(QRect(540, 160, 83, 25));
        select = new QPushButton(centralwidget);
        select->setObjectName(QString::fromUtf8("select"));
        select->setGeometry(QRect(540, 180, 83, 25));
        down_arrow = new QPushButton(centralwidget);
        down_arrow->setObjectName(QString::fromUtf8("down_arrow"));
        down_arrow->setGeometry(QRect(540, 200, 83, 25));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        contact->setText(QCoreApplication::translate("MainWindow", "Contact", nullptr));
        treatment->setText(QCoreApplication::translate("MainWindow", "Treatment", nullptr));
        contact_lost->setText(QCoreApplication::translate("MainWindow", "Contact Lost", nullptr));
        power->setText(QCoreApplication::translate("MainWindow", "Power", nullptr));
        display->setTitle(QCoreApplication::translate("MainWindow", "Display", nullptr));
        new_session->setText(QCoreApplication::translate("MainWindow", "NEW SESSION", nullptr));
        session_log->setText(QCoreApplication::translate("MainWindow", "SESSION LOG", nullptr));
        time_date->setText(QCoreApplication::translate("MainWindow", "TIME AND DATE", nullptr));
        timer->setText(QCoreApplication::translate("MainWindow", "0:00", nullptr));
        menu->setText(QCoreApplication::translate("MainWindow", "Menu", nullptr));
        up_arrow->setText(QCoreApplication::translate("MainWindow", "\342\206\221", nullptr));
        select->setText(QCoreApplication::translate("MainWindow", "Select", nullptr));
        down_arrow->setText(QCoreApplication::translate("MainWindow", "\342\206\223", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
