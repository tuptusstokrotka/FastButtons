#ifndef FastButton_H
#define FastButton_H
#pragma once

#include <Arduino.h>    // millis()

enum class ButtonState{
    unknown,
    released,
    pressed,
    held,
    double_press,
};

enum class InputType{
    digital,
    analog
};

#define NORMALLY_OPEN   0
#define NORMALLY_CLOSED 1

class FastButton{
public:

private:
    // Config
    //--------------------------------
    int pinnum;
    //TODO ADD the DDR PORT AND PIN
    //TODO ADD DEBOUNCE TIME

    InputType inputType = InputType::digital;       // If set to analog, the button will be read analog input and the threshold will be used to determine the state
    int analogThreshold = 800;                      // When using analog as a button define the threshold.

    bool stateRepeat[3] = {true, false, false};     // GetState() will repeat state. By default: released `true` | pressed `false` | held `false`
    bool waitForRelease = false;                    // Return state only with the button release. Note: ignores the stateRepeat

    // Monitoring
    //--------------------------------
    bool releasedState = NORMALLY_OPEN;             // GPIO state on RELEASE
    ButtonState last_State;                         // Last button state

    // Hold button trigger and press time
    unsigned long hold_ms;                          // Time required to trigger HOLD
    unsigned long press_time = 0;                   // Press time in ms

    // Double press
    unsigned long doublePress_ms = 1000;            // Time of 1st press - 2nd press must be triggered within (doublePress_ms)
    unsigned int press_count = 0;                   // Number of presses

    ButtonState GetState_wfr(ButtonState state);    // Wait For Release
    ButtonState GetState_sr(ButtonState state);     // State Repeat
public:

    FastButton(int pin, unsigned long hold = 400, bool wfr = false);
    //TODO Add constructor for the DDR and PIN config
    ~FastButton();

    inline void ConfigInputType(InputType type) {inputType = type;}
    inline void ConfigAnalogThreshold(unsigned int reading) {analogThreshold = reading;}

    inline void ConfigReleasedState(bool state) {releasedState = state;}
    inline void ConfigNormallyOpen(void) {ConfigReleasedState(NORMALLY_OPEN);}
    inline void ConfigNormallyClosed(void) {ConfigReleasedState(NORMALLY_CLOSED);}

    inline void ConfigStateRepeat(bool released, bool pressed, bool held) {stateRepeat[0] = released; stateRepeat[1] = pressed; stateRepeat[2] = held;}
    inline void ConfigWaitForRelease(bool enable) {waitForRelease = enable;}
    inline void ConfigHeldTime(unsigned long time) {hold_ms = time;}

    inline void ConfigDoublePressTime(unsigned long time) {doublePress_ms = time;}

    // Raw Data
    bool isPressed(void);
    bool isReleased(void) {return !isPressed();}
    bool isHeldFor(unsigned long time) {return isPressed() && (millis() - press_time >= time);}
    bool isDoublePressed(void) {return isPressed() && (millis() - press_time >= doublePress_ms);}

    // Get State
    ButtonState GetState();
};

#endif