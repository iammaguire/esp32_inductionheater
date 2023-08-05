struct CircularMeter {
  TFT_eSprite sprite;
  int centerX;
  int centerY;
  int radius;
  uint16_t backgroundColor;
  uint16_t lightColor;
  int currentValue;   // Current value of the meter
  int targetValue;    // Target value to interpolate towards
  int interpolationDuration; // Duration in milliseconds for the interpolation
  unsigned long startTime; // Time when the interpolation started

  CircularMeter(int x, int y, int r, uint16_t bg, uint16_t lc)
      : sprite(TFT_eSprite(&tft)), centerX(x), centerY(y), radius(r),
        backgroundColor(bg), lightColor(lc), currentValue(0), targetValue(0),
        interpolationDuration(1000), startTime(0) {
    sprite.setColorDepth(16);
    sprite.createSprite(2 * r + 1, 2 * r + 1);
  }

  // Set the target value and start interpolation
  void setTargetValue(int target, int duration) {
    int tval = constrain(target, 0, 100);
    if (tval != targetValue) {
      Serial.println(tval);
      targetValue = tval;
      interpolationDuration = max(duration, 100); // Ensure a minimum duration of 100ms
      startTime = millis();
    }
  }

void update() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  // Check if the interpolation is still ongoing
  if (elapsedTime < interpolationDuration) {
    // Apply the ease-out interpolation function
    float progress = static_cast<float>(elapsedTime) / interpolationDuration;
    float easedProgress = cubicBezierEaseOut(progress);

    // Calculate the interpolated value based on the eased progress
    currentValue = currentValue + round(easedProgress * -(currentValue - targetValue));
  } else {
    currentValue = targetValue; // Interpolation completed, set to target value
  }

  draw(); // Redraw the meter with the updated value
}

// Quadratic ease-out interpolation function
float cubicBezierEaseOut(float t) {
  // Define control points (adjust as needed)
  const float p0 = 0.0;
  const float p1 = 0.3;
  const float p2 = 0.7;
  const float p3 = 1.0;

  // Calculate the cubic bezier easing value
  float u = 1.0 - t;
  float tt = t * t;
  float uu = u * u;
  float uuu = uu * u;
  float ttt = tt * t;

  float p = p0 * uuu;
  p += 3.0 * p1 * uu * t;
  p += 3.0 * p2 * u * tt;
  p += p3 * ttt;

  return p;
}


  // Draw the circular meter with the current value
  void draw() {
    sprite.fillSprite(backgroundColor);

    float angle = currentValue * 3.6;

    for (int i = 0; i < 360; i++) {
      float startAngle = radians(90 - i);
      float endAngle = radians(91 - i);

      int xStart = radius + cos(startAngle) * radius;
      int yStart = radius - sin(startAngle) * radius;
      int xEnd = radius + cos(endAngle) * radius;
      int yEnd = radius - sin(endAngle) * radius;

      sprite.drawLine(xStart, yStart, xEnd, yEnd, i <= angle ? lightColor : TFT_WHITE);
    }

    sprite.setFreeFont(FF6); 
    String text = String(currentValue) + "%";
    int textWidth = sprite.textWidth(text);
    int textHeight = sprite.fontHeight();
    int textX = radius - textWidth / 2;
    int textY = radius + textHeight / 4;

    sprite.setTextColor(TFT_WHITE);
    sprite.setCursor(textX, textY);
    sprite.print(text);

    sprite.pushSprite(centerX - radius, centerY - radius);
  }
};
