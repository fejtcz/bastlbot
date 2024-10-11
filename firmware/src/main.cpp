#include <Arduino.h>

// Pins definitions
#define BUZZER_PIN 27

// Beep by the integrated buzzer
void beep(int frequency, int duration)
{
  tone(BUZZER_PIN, frequency, duration);
}

// Play startup sound
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
  Serial.begin(115200);
  startupSound();
}

void loop()
{
}