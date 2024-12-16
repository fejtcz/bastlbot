#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// PIN's definitions
#define PIN_BUZZER 27
#define PIN_SDA 21
#define PIN_SCL 22

// OLED displat definitions
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

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

  // OLED init and check
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    printMessage("OLED allocation failed");
    return;
  }

  // OLED Startup message
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, 10);
  display.println("BastlBot");
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.println("Bat. voltage: 0.00V"); // TODO - dodelat mereni napeti baterie
  display.display();

  // Play startup sound
  playStartupSound();
}

void loop()
{
}