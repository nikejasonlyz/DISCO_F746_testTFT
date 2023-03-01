/*
 * @description: 
 * @param: 
 * @return: 
 * @author: Universe
 */
// yjTask.hpp
//
#include <Arduino.h>

class YjTask
{
public:
  YjTask(uint32_t deltaT = 500) : dt(deltaT)
  {
    tNext = dt;
  }
  void poll()
  {
    if (millis() >= tNext)
    {
      loop();
      tNext += dt;
    }
  }
  virtual void loop() = 0;

protected:
  const uint32_t dt;
  volatile uint32_t tNext;
};

class TaskLed : public YjTask
{
public:
  TaskLed(uint8_t vpin, uint32_t deltaT = 500) : YjTask(deltaT), pin(vpin)
  {
    pinMode(pin, OUTPUT);
  }
  void loop()
  {
    digitalWrite(pin, !digitalRead(pin));
  }

private:
  uint8_t pin;
};