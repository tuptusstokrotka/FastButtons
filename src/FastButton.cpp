#include "FastButton.h"

FastButton::FastButton(int pin, unsigned long hold, bool wfr){
    pinMode(pin , INPUT);
    this->pinnum = pin;
    this->hold_ms = hold;
    this->waitForRelease = wfr;
}

FastButton::~FastButton(){}

bool FastButton::isPressed(void){
    //TODO make it baremetal

    // analog input
    if(inputType == InputType::analog){
        return releasedState == NORMALLY_OPEN ?
            analogRead(pinnum) >= analogThreshold :
            analogRead(pinnum) <= analogThreshold;
    }
    // digital input
    else{
        return releasedState == NORMALLY_OPEN ?
            digitalRead(pinnum) == HIGH :
            digitalRead(pinnum) == LOW;
    }
}

ButtonState FastButton::GetState_wfr(ButtonState state){
    ButtonState temp = last_State;                  // Save last state to temp
    last_State = state;                             // Update last state

    if(state == ButtonState::released)              // If the button was released
        return temp;                                // Return last state
    return ButtonState::unknown;                    // Return undefined
}

ButtonState FastButton::GetState_sr(ButtonState state){
    bool newState = (state != last_State);          // Check if the state has changed

    if(newState){                                   // Always return the new state
        last_State = state;
        return state;
    }

    switch (state) {                                // Return the state if the stateRepeat is true
        case ButtonState::released:
            return (stateRepeat[0]) ? state : ButtonState::unknown;
        case ButtonState::pressed:
            return (stateRepeat[1]) ? state : ButtonState::unknown;
        case ButtonState::held:
            return (stateRepeat[2]) ? state : ButtonState::unknown;
        default:
            return ButtonState::unknown;
    }
}

ButtonState FastButton::GetState(){
    ButtonState state = ButtonState::unknown;       // Current state

    /* Get current state */
    if(isPressed()){                                // Button is pressed
        state = ButtonState::pressed;

        if(press_time == 0){                        // just been pressed
            press_time = millis();
        }
        else if(millis() - press_time >= hold_ms){  // held for long enough
            state = ButtonState::held;
        }
    }
    else{                                           // Button is released
        state = ButtonState::released;
        press_time = 0;
    }

    /* Double Press */
    //TODO

    /* State with Wait For Release */
    if(waitForRelease)
        return GetState_wfr(state);

    /* State with State Repeat */
    return GetState_sr(state);
}
