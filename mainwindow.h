#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "neureset.h"

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
    Neureset neureset;

    void updateDisplay(MenuOption option);

public slots:
    void onUpArrowPressed();
    void onDownArrowPressed();
    void onSelectPressed();
    void changeDisplay(MenuOption option = Menu);
    void pauseSession();
    void startSession();

private:
    Ui::MainWindow *ui;
    void highlightCurrentSelection();
    void updateDateTimeDisplay();

    MenuOption currentSelection;
    MenuOption currentDisplay;
};
#endif // MAINWINDOW_H
