#include "display.h"
#include <QDebug> // Include for qDebug()

Display::Display(QWidget *parent) : QWidget(parent), currentSelection(Menu), currentDisplay(Menu)
{
    qDebug() << "Display initialized with Menu";
    highlightCurrentSelection();
    updateDisplay(Menu);
}

Display::~Display()
{
    qDebug() << "Display destroyed";
}

void Display::onUpArrowPressed()
{
    if (currentSelection == Menu) {
        qDebug() << "onUpArrowPressed: Already at the first item, no action taken";
        return; // No action if it's already at the first item
    }
    currentSelection = static_cast<MenuOption>(currentSelection - 1);
    qDebug() << "onUpArrowPressed: Moved up, currentSelection:" << currentSelection;
    highlightCurrentSelection();
}

void Display::onDownArrowPressed()
{
    if (currentSelection == SessionLog) {
        qDebug() << "onDownArrowPressed: Already at the last item, no action taken";
        return; // No action if it's already at the last item
    }
    currentSelection = static_cast<MenuOption>(currentSelection + 1);
    qDebug() << "onDownArrowPressed: Moved down, currentSelection:" << currentSelection;
    highlightCurrentSelection();
}

void Display::onSelectPressed()
{
    qDebug() << "onSelectPressed: Option selected, currentSelection:" << currentSelection;
    changeDisplay(currentSelection);
}

void Display::highlightCurrentSelection()
{
    // Logic to highlight the current menu item based on currentSelection
    qDebug() << "highlightCurrentSelection: Current selection highlighted, currentSelection:" << currentSelection;
}

void Display::changeDisplay(MenuOption option)
{
    currentDisplay = option;
    qDebug() << "changeDisplay: Display changed, currentDisplay:" << option;
    updateDisplay(option);
}

void Display::updateDisplay(MenuOption option)
{
    // Reset visibility of all display group items
    // Implement the logic to update the display based on the 'option'
    // For instance, if 'option' is Time, make sure only the time_date is visible
    // and appropriately updated.

    qDebug() << "updateDisplay: Updating display for option:" << option;
    updateDateTimeDisplay();
}

void Display::updateDateTimeDisplay()
{
    // Logic to update date and time display
    qDebug() << "updateDateTimeDisplay: Date and Time display updated";
}
