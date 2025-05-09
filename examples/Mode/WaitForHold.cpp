#include <Arduino.h>
#include "FastButton.h"

#define BTN_PIN 2

// button(PIN_NUM, HOLD_MS, Wait For Release)
FastButton button(BTN_PIN, 2000, false);

// DEBUG
char* GetStateStr(ButtonState state){
    char* state_str[] = {
        "unknown",
        "released",
        "pressed",
        "held"
    };
    return state_str[static_cast<int>(state)];
}

void setup(){
    Serial.begin(115200);

    // Button Config
    button.ConfigWaitForHold(true);
    button.ConfigStateRepeat(true, false, true); // To repeat released and held
}

void loop(){
    ButtonState state = button.GetState();
    Serial.println(GetStateStr(state));
    delay(50);
}
