
## A potentiometer lock component for esp32
This component implements the behaviour of a physical potentiometer lock (rotating lock like in old safes) connected to the esp32 using a potentiometer.

### Remarks
In order to know the limits of each code or for testing purposes, the current code is printed in the logs.

### TODO
- Use interrupts instead of a task to check for the arrival of a new code (the task would only be activated when an interrupt gets a new code)
- Verify valid code in depth before setting it:
    - Check if no code\_char is >= CONFIG\_POSITIONS\_NB
    - Check that all even positionned code\_char are higher than the preceding one and all odd positionned code\_char are smaller than the preceding one (even & odd here are not accounting the first 0 code\_char appended to the code to simplify the algorithms)
- Maybe use the position 0 to reset the current code (the user would turn the potentiometer to the end to reset the input code and start again)
