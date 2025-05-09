#include "FastButton.h"

FastButton::FastButton(int pin, unsigned long hold, bool wfr){
    pinMode(pin , INPUT);
    this->pinnum = pin;
    this->hold_ms = hold;
    this->waitForRelease = wfr;
}

FastButton::~FastButton(){}

/**
 * @brief Check the button GPIO to determine the state
 */
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

/**
 * @brief
 * @note WaitForRelease will ignore every other mode. State is always send on release.
 * @param state raw state
 * @return current state
 */
ButtonState FastButton::GetState_wfr(ButtonState state){
    ButtonState temp = last_State;                  // Save last state to temp
    last_State = state;                             // Update last state

    if(state == ButtonState::released)              // If the button was released
        return temp;                                // Return last state
    return ButtonState::unknown;                    // Return undefined
}

/**
 * @brief
 * @note WaitForHold sends `pressed` always once. `held` repeating can be configured via `StateRepeat`
 * @param state raw state
 * @return current state
 */
ButtonState FastButton::GetState_wfh(ButtonState state){
    static bool pressed = false;

    if(state == ButtonState::pressed){              // Trigger the Wait For Hold
        pressed = true;                             // Set flag for wfh
        return ButtonState::unknown;
    }

    if(pressed && state != ButtonState::pressed){   // Wait For Hold (or release)
        pressed = false;                            // Reset flag for wfh
        if(state == ButtonState::released)
            return ButtonState::pressed;            // Return pressed - If the button was released
        // else
        //     return ButtonState::held;               // Return held - If the button was held
    }
    // Use State Repeat for the rest of the states
    return GetState_sr(state);
}

/**
 * @brief
 * @note WaitForHold will never send `pressed` more than once.
 * @param state raw state
 * @return current state
 */
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


/**
 * @brief Read the button state using one of the configuration.
 *
 * + `WaitForRelease`
 *
 * + `WaitForHold`
 *
 * + `StateRepeat` (default)
 *
 * @return Current button state
 */
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
    /* State with Wait For Hold */
    else if(waitForHold)
        return GetState_wfh(state);
    /* State with State Repeat */
    else
        return GetState_sr(state);
}
