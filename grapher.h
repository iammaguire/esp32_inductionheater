TFT_eSPI tft = TFT_eSPI();
TFT_eSprite infoText = TFT_eSprite(&tft);
TFT_eSprite graph1 = TFT_eSprite(&tft);

static uint32_t plotTime = millis();
static float gx = 0.0, gy = 0.0;
static float delta = 7.0;

int grid = 0;
int graphMaxY = 200;
int graphMaxX = 100;
int graphWidth = 100*2;
int graphHeight = 63*2;
int lastTargetX = graphWidth-2;
int lastTargetY = graphHeight-2;
int lastTempX = graphWidth-2;
int lastTempY = graphHeight-2;
int lastOutX = graphWidth-2;
int lastOutY = graphHeight-2;
int graphX = 5;
int graphY = 5;
char infoStr[50];

int tempColor() {
  if(temperature < 40) {
    return TFT_GREEN; 
  } else if(temperature < 80) {
    return TFT_YELLOW;
  } else {
    return TFT_RED;
  }
}

void drawUI() { 
  if (millis() - plotTime >= 100) {
    plotTime = millis();

    //graph1.drawFastVLine(graphWidth-1,graphHeight-temperature,1,TFT_YELLOW);
    //graph1.drawFastVLine(graphWidth-1,graphHeight-temperature,1,tempColor());
    graph1.drawLine(lastTargetX, lastTargetY, graphWidth-1, graphHeight-target, TFT_GOLD);
    graph1.drawLine(lastTempX, lastTempY, graphWidth-1, graphHeight-temperature, tempColor());
    graph1.drawLine(lastOutX, lastOutY, graphWidth-1, graphHeight-output_outer-60, TFT_WHITE);
    graph1.scroll(-1, 0);
    graph1.pushSprite(graphX, graphY);
    
    lastTargetX = graphWidth-1;
    lastTargetY = graphHeight-target;
    lastTempX = graphWidth-1;
    lastTempY = graphHeight-temperature;
    lastOutX = graphWidth-1;
    lastOutY = graphHeight-output_outer-60;
    Serial.println(output_outer);
    grid++;
    
    if (grid >= 25)
    {
      grid = 0;
      graph1.drawFastVLine(graphWidth-1, 0, graphHeight, TFT_BLUE); 
    }
    
    int numVSpaces = 5;
    for (int p = graphHeight / numVSpaces; p < graphHeight; p += graphHeight / numVSpaces) graph1.drawPixel(graphWidth-1, p, TFT_BLUE);

    gx += 1.0;
    if (gx > 100.0) {
      gx = 0.0;
    }
  }

  infoText.setFreeFont(FF6); 
  sprintf(infoStr, "%.02f", temperature);
  infoText.drawString(infoStr, 0, 0);
  sprintf(infoStr, "%.02f", target);
  infoText.drawString(infoStr, 0, 35);
  infoText.drawFastVLine(graphX + graphWidth + 1, graphY, graphHeight, TFT_BLUE);
  infoText.pushSprite(graphX+10, graphY+graphHeight+20);
  
  graph1.drawPixel(graphWidth-1, graphHeight-1, TFT_BLUE);
  graph1.drawFastVLine(1, 1, graphHeight-1, TFT_BLUE);
  tft.drawFastVLine(graphX+graphWidth,graphY-1,graphHeight-1,TFT_BLUE);
  tft.drawFastHLine(graphX+2,graphY-1,graphWidth-1,TFT_BLUE);
}

void setupUI() {
  tft.init();
  tft.setFreeFont(FF6); 
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
 // tft.loadFont(hefeng40);
  //tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  //tft.setTextSize(1);
  //tft.setAttribute(UTF8_SWITCH, true);
   // tft.drawString("\uf103", 100, 100);
  
  infoText.setColorDepth(8);
  infoText.createSprite(graphWidth, 200);
  infoText.fillSprite(TFT_BLACK);

  graph1.setColorDepth(8);
  graph1.createSprite(graphWidth, graphHeight+1);
  graph1.fillSprite(TFT_BLACK); // Note: Sprite is filled with black when created
  graph1.drawFastHLine(0, 0, graphWidth+1, TFT_BLUE);
  graph1.drawFastHLine(0, graphHeight-1, graphWidth, TFT_BLUE);
}
