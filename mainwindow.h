#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <string>
#include "neureset.h"
#include <QTimeEdit>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum MenuOption {
        NewSession,
        SessionLog,
        TimeDate,
        Menu
    };
    Neureset* neureset;

    void updateDisplay(MenuOption option);

public slots:
    void onUpArrowPressed();
    void onDownArrowPressed();
    void onSelectPressed();
    void changeDisplay(MenuOption option = Menu);
    void pauseSession();
    void startSession();
    void lightChange(int);
    void stopSession();
    void resetLight();

private:
    Ui::MainWindow *ui;
    QDateTime currentDateTime;
    void highlightCurrentSelection();
    void updateDateTimeDisplay();
    void updateSessionLogDisplay();
    void startNeuresetSession();
    void updateTimer();

    MenuOption currentSelection;
    MenuOption currentDisplay;

    QTimer *timer;
};
#endif // MAINWINDOW_H
