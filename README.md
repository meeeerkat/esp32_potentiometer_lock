
## A potentiometer lock component for esp32
This component implements the behaviour of a physical potentiometer lock (rotating lock like in old safes) connected to the esp32 using a potentiometer.

### Remarks
In order to know the limits of each code or for testing purposes the current code is printed in the logs.

### TODO
- Use interrupts instead of a task to check for the arrival of a new code (the task would only be activated when an interrupt gets a new code)
