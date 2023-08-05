#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN 35
#define ROTARY_ENCODER_B_PIN 34
#define ROTARY_ENCODER_BUTTON_PIN 21
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

void rotary_onButtonClick()
{
    static unsigned long lastTimePressed = 0; // Soft debouncing
    if (millis() - lastTimePressed < 500)
    {
            return;
    }
    lastTimePressed = millis();
    Serial.print("button pressed ");
    Serial.print(millis());
    Serial.println(" milliseconds after restart");
}

void rotary_loop()
{
    //dont print anything unless value changed
    if (rotaryEncoder.encoderChanged())
    {
        int value = rotaryEncoder.readEncoder();
        Serial.print("V:");
        Serial.println(value);
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {
        rotary_onButtonClick();
    }
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void setupRotEncoder() {
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    //set boundaries and if values should cycle or not
    //in this example we will set possible values between 0 and 1000;
    bool circleValues = false;
    rotaryEncoder.setBoundaries(0, 1000, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

    /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
    //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
    rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}