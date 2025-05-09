#include <Arduino.h>
#include "FastButton.h"

#define BTN_PIN_A 2
#define BTN_PIN_B 3

// button(PIN_NUM, HOLD_MS, Wait For Release)
FastButton button_a(BTN_PIN_A, 2000, false);
FastButton button_b(BTN_PIN_B, 2000, false);

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

    // Button A Config - this will repeat released and hold
    button_a.ConfigStateRepeat(true, false, true);
    button_a.ConfigNormallyClosed();

    // Button B Config - this will repeat released
    button_b.ConfigStateRepeat(true, false, false);
    button_b.ConfigNormallyClosed();

}

void loop(){
    ButtonState state_a = button_a.GetState();
    Serial.println(GetStateStr(state_a));

    ButtonState state_b = button_b.GetState();
    Serial.println(GetStateStr(state_b));
    delay(50);
}
