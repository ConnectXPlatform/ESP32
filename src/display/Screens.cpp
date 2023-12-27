#include "Screens.h"

#pragma region Screen
void Screen::prepare()
{
    displayHandler.clearScreen();
    prepareImpl();
}

void Screen::dispose()
{
    disposeImpl();
}
#pragma endregion

#pragma region SetupScreen
void SetupScreen::addStep(const char *step)
{
    displayHandler.showLabel(step, MediumText, 5, currentY, 255, 255, 255);
    lastStep = strdup(step);
}

void SetupScreen::stepCompleted()
{
    uint8_t numLines = displayHandler.showLabel(lastStep, MediumText, 5, currentY, 0, 255, 0);
    free(lastStep);
    currentY += 30 + (numLines - 1) * 15;
    lastStep = nullptr;
}

void SetupScreen::prepareImpl()
{
    currentY = 10;
}

void SetupScreen::disposeImpl()
{
    if (lastStep != nullptr)
    {
        free(lastStep);
        lastStep = nullptr;
    }
}
#pragma endregion
