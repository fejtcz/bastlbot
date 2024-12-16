#include <Arduino.h>

// PIN's definitions
#define PIN_BUZZER 27

/*
 * Print message to Serial
 * @param message - message to be printed
 */
void printMessage(String message)
{
  Serial.println(message);
}

/*
 * Beep by the intergrated buzzer
 * @param frequency - frequency of the beep
 * @param duration - duration of the beep
 */
void beep(int frequency, int duration)
{
  tone(PIN_BUZZER, frequency, duration);
}

/*
 * Startup sound
 */
void playStartupSound()
{
  beep(1000, 150);
  delay(50);
  beep(2000, 150);
  delay(50);
  beep(3000, 150);
  delay(50);
  beep(2000, 350);
}

void setup()
{
  // Serial output init
  Serial.begin(115200);
  printMessage("***** BastlBot *****");
  printMessage("BastlBot starting...");

  // Play startup sound
  playStartupSound();
}

void loop()
{
}