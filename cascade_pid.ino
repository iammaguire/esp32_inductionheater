#include <Adafruit_MLX90614.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>  
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>           
#include "hefeng.h"
#include "pid.h"
#include "Free_Fonts.h"
double output_outer;
#include "grapher.h"
#include "circularmeter.h"

#define DEVICE "ESP32"
#define BTN1 33
#define BTN2 5
#define RLY 26

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

CircularMeter tempMeter(320-55, 50, 40, TFT_BLACK, TFT_BLUE);
CircularMeter targetMeter(320-55, 150, 40, TFT_BLACK, TFT_BLUE);

PIDController pid;

bool buttonState1 = false;     // Tracks the state of BTN1
bool buttonState2 = false;     // Tracks the state of BTN2
bool lastButtonState1 = false; // Tracks the previous state of BTN1
bool lastButtonState2 = false; // Tracks the previous state of BTN2

void setup() {
  Serial.begin(115200);

  setupUI();
  
  pid = {
    PID_KP, PID_KI, PID_KD,
    PID_TAU,
    PID_LIM_MIN, PID_LIM_MAX,
    PID_LIM_MIN_INT, PID_LIM_MAX_INT,
    SAMPLE_TIME_S
  };

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  }

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(RLY, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    pid.Kp = Serial.parseFloat();
    Serial.read();
    Serial.println("---------------------------------------");
    Serial.println(pid.Kp);
    Serial.println("---------------------------------------");
  }

  buttonState1 = digitalRead(BTN1);
  buttonState2 = digitalRead(BTN2);

  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      target += 10;
    }
    delay(50); // Debounce delay
  }

  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == LOW) {
      target -= 10;
    }
    delay(50); // Debounce delay
  }

  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;

  temperature = mlx.readObjectTempC();//40.5 * sin(0.023957 * millis()/40) + 59.5;

  unsigned long currentMillis = millis();

  static unsigned long previousMillis = 0;
  static int curve_index = 0;
  
  // Check if it's time to update the target temperature
  if (currentMillis - previousMillis >= SAMPLE_TIME_S * 1000) {
    previousMillis = currentMillis;
    
    //target = temperature_curve[curve_index];

    curve_index++;
    if (curve_index >= curve_length) {
      curve_index = 0;
    }
  }

  double error_outer = target - temperature;
  output_outer = PIDController_Update(&pid, target, temperature);

  digitalWrite(RLY, output_outer > 0 ? HIGH : LOW);

  /*Serial.print((target - temperature));
  Serial.print(" ");
  Serial.print(target);
  Serial.print(" ");
  Serial.print(temperature);
  Serial.print(" ");
  Serial.print(output_outer);
  Serial.println("");*/

  drawUI();

  tempMeter.value = target / graphHeight * 100;
  tempMeter.draw();
  targetMeter.value = 101 - (abs(target - temperature)) / 100.0 * 100;
  targetMeter.draw();

  delay(1);
}

