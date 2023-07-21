struct CircularMeter {
  TFT_eSprite sprite;
  int centerX;
  int centerY;
  int radius;
  uint16_t backgroundColor;
  uint16_t lightColor;
  int value;

  CircularMeter(int x, int y, int r, uint16_t bg, uint16_t lc)
      : sprite(TFT_eSprite(&tft)), centerX(x), centerY(y), radius(r),
        backgroundColor(bg), lightColor(lc), value(0) {
    sprite.setColorDepth(16);
    sprite.createSprite(2 * r + 1, 2 * r + 1);
  }

  void draw() {
    sprite.fillSprite(backgroundColor);

    float angle = value * 3.6;

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
    String text = String(value) + "%";
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