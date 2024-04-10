#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>

class Display : public QWidget
{
    Q_OBJECT

public:
    explicit Display(QWidget *parent = nullptr);
    ~Display();

    enum MenuOption {
        Menu,
        Time,
        NewSession,
        SessionLog
    };

    void updateDisplay(MenuOption option);

public slots:
    void onUpArrowPressed();
    void onDownArrowPressed();
    void onSelectPressed();

private:
    void highlightCurrentSelection();
    void changeDisplay(MenuOption option);
    void updateDateTimeDisplay();

    MenuOption currentSelection;
    MenuOption currentDisplay;
};

#endif // DISPLAY_H
