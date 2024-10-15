#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pins definitions
#define SDA_PIN 21
#define SCL_PIN 22
#define BUZZER_PIN 27
#define VOLTAGE_PIN 34

// Display definitions
#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_RESET -1
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);

/*
 * Beep by the integrated buzzer
 * @param frequency - frequency of the sound
 * @param duration - duration of the sound
 */
void beep(int frequency, int duration)
{
  tone(BUZZER_PIN, frequency, duration);
}

/*
 * Play startup sound
 */
void startupSound()
{
  beep(1000, 150);
  delay(50);
  beep(2000, 150);
  delay(50);
  beep(3000, 150);
  delay(50);
  beep(2000, 150);
}

void setup()
{
  // Serial output
  Serial.begin(115200);

  // Display check
  if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS))
  {
    Serial.println(F("Display allocation failed!"));
    return;
  }

  // Voltage check
  pinMode(VOLTAGE_PIN, INPUT);
  float voltageValue = analogReadMilliVolts(VOLTAGE_PIN) * 1.5625 / 1000.0;

  // Welcome message
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, 10);
  display.println("BastlBot");
  display.setTextSize(1);
  display.setCursor(40, 30);
  display.println("(Stepper)");
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.println("Bat. voltage: " + String(voltageValue) + "V");
  display.display();

  // Startup sound
  startupSound();
}

void loop()
{
}