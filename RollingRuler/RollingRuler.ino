#include "Adafruit_seesaw.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SS_SWITCH        24

#define SEESAW_ADDR          0x36

Adafruit_seesaw ss;

int32_t encoder_position;

int counter = 0;

const int buttonPin = 11;
int buttonState = 0;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;   

double distance = 0;
float wheelDiameter = 4.23;
bool isMeasuring = false;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);

  Serial.println("Looking for seesaw!");
  
  if (! ss.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    while(1) delay(10);
  }
  Serial.println("seesaw started");

  encoder_position = ss.getEncoderPosition();

  Serial.println("Turning on interrupts");
  delay(10);
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  ss.enableEncoderInterrupt();

  pinMode(buttonPin, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Rolling\nRuler");
  display.display();
  Serial.println("Rolling Ruler");
}

void loop() {
  int32_t new_position = ss.getEncoderPosition();
  if (encoder_position != new_position) {
    encoder_position = new_position;
    if(isMeasuring){
      counter++;

      distance = counter*wheelDiameter*PI/24;
      
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(String(distance) + " cm.           ");
      display.display();
    }
  }

  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        if(isMeasuring){
          isMeasuring = false;

          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("Measurement Done\n" + String(distance) + " cm.");
          display.display();
          distance = 0;
          counter = 0;
        }
        else {
          distance = 0;
          isMeasuring = true;
          counter = 0;

          display.clearDisplay();
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 0);
          display.println("Started\nMeasurement");
          display.display();
        }
      }
    }
  }
  lastButtonState = reading;
}