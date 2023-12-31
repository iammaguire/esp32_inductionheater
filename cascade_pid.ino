#include <Adafruit_MLX90614.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include <ezButton.h>
#include "hefeng.h"
#include "pid.h"
#include "Free_Fonts.h"
double output_outer;
#include "grapher.h"
#include "circularmeter.h"
#include "rotencoder.h"

#define DEVICE "ESP32"
#define BTN1 33
#define BTN2 5
#define RLY 26

ezButton button1(BTN1);
ezButton button2(BTN2);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

CircularMeter tempMeter(320 - 55, 50, 40, TFT_BLACK, TFT_BLUE);
CircularMeter targetMeter(320 - 55, 150, 40, TFT_BLACK, TFT_BLUE);

PIDController pid;

bool buttonState1 = false;      // Tracks the state of BTN1
bool buttonState2 = false;      // Tracks the state of BTN2
bool lastButtonState1 = false;  // Tracks the previous state of BTN1
bool lastButtonState2 = false;  // Tracks the previous state of BTN2

bool heaterOn = false;

double heatStartTemp;
unsigned long heatPeakPointStartTime = -1;
const unsigned long peakDuration = 6000;  // Stay at set temp for n seconds after reaching

unsigned long debounceTimeB1 = 0;
unsigned long debounceTimeB2 = 0;
unsigned long debounceDualTime = 0;
unsigned long debounceDelay = 50;
unsigned long dualPressDelay = 100;

const double maxTemp = 150.0;
const double minTemp = 50.0;

const int meterDelay = 200;

void setup() {
  Serial.begin(115200);

  setupUI();
  setupRotEncoder();
  tempMeter.setTargetValue(target / maxTemp * 100, 3000);

  pid = {
    PID_KP, PID_KI, PID_KD,
    PID_TAU,
    PID_LIM_MIN, PID_LIM_MAX,
    PID_LIM_MIN_INT, PID_LIM_MAX_INT,
    SAMPLE_TIME_S
  };

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  }

  //pinMode(BTN1, INPUT_PULLUP);
  //pinMode(BTN2, INPUT_PULLUP);
  button1.setDebounceTime(debounceDelay);
  button2.setDebounceTime(debounceDelay);
  pinMode(RLY, OUTPUT);
}

void loop() {
  button1.loop();
  button2.loop();
  rotary_loop();

  if (Serial.available()) {
    pid.Kp = Serial.parseFloat();
    Serial.read();
    Serial.println("---------------------------------------");
    Serial.println(pid.Kp);
    Serial.println("---------------------------------------");
  }

  if (button1.isReleased() && button2.isReleased()) {
    heaterOn = true;
    heatPeakPointStartTime = -1;
    heatStartTemp = temperature;
  } else {
    if (button1.isReleased() && !heaterOn && target + 5 <= maxTemp) {
      target += 5;
    }
    if (button2.isReleased() && target - 5 >= minTemp) {
      if (heaterOn) {
        heaterOn = false;
        heatPeakPointStartTime = -1;
      } else if (target - 5 >= 0) target -= 5;
    }
  }

  temperature = mlx.readObjectTempC();  //40.5 * sin(0.023957 * millis()/40) + 59.5;

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

  /*Serial.print((target - temperature));
  Serial.print(" ");
  Serial.print(target);
  Serial.print(" ");
  Serial.print(temperature);
  Serial.print(" ");
  Serial.print(output_outer);
  Serial.println("");*/

  drawUI();

  tempMeter.setTargetValue(target / maxTemp * 100, meterDelay);
  tempMeter.update();

  if (heaterOn) {
    if (heatPeakPointStartTime != -1 && millis() - heatPeakPointStartTime >= peakDuration) {
      heaterOn = false;
      heatPeakPointStartTime = -1;
    }

    if (output_outer <= 0 && heatPeakPointStartTime == -1) heatPeakPointStartTime = millis();

    double range = abs(heatStartTemp - target);
    targetMeter.setTargetValue(((temperature - heatStartTemp) / range) * 100, meterDelay);
    targetMeter.update();
    digitalWrite(RLY, output_outer > 0 ? HIGH : LOW);
    targetMeter.lightColor = heatPeakPointStartTime != -1 ? TFT_GREEN : TFT_RED;
  } else {
    targetMeter.setTargetValue(0, 30000);
    targetMeter.update();
    digitalWrite(RLY, LOW);
    targetMeter.lightColor = TFT_BLUE;
  }

  delay(1);
}
