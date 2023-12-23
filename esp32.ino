#include "src/input/buttons/ButtonHandler.h"
#include "src/utils/TasksQueue.h"
#include "src/utils/BackgroundTask.h"

ButtonHandler powerButton(GPIO_NUM_9, 10000);
ButtonHandler resetButton(GPIO_NUM_10, 10000);

void initializeButtons()
{
  // Use interrupts to listen to the buttons' pins.
  noInterrupts();
  attachInterrupt(
      digitalPinToInterrupt(powerButton.getButtonPin()), []()
      { powerButton.pinStateChanged(); },
      CHANGE);
  attachInterrupt(
      digitalPinToInterrupt(resetButton.getButtonPin()), []()
      { resetButton.pinStateChanged(); },
      CHANGE);
  interrupts();

  // Create an infinite tasks to constantly update the buttons.
  static BackgroundTask powerButtonTask([]()
                                        { powerButton.update(); },
                                        500);
  static BackgroundTask resetButtonTask([]()
                                        { resetButton.update(); },
                                        500);
  TasksQueue::getInstance().enqueueTask(powerButtonTask);
  TasksQueue::getInstance().enqueueTask(resetButtonTask);
}

void setup()
{
  TasksQueue::getInstance().initialize(ARDUINO_RUNNING_CORE);
  initializeButtons();
}

void loop()
{
}
