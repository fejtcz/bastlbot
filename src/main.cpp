#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MotorShield.h>

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

// Motor shield and motors definitions
#define MOTOR_SHIELD_ADDRESS 0x40
const int FORWARD_MOVE_SPEED = 50;
const int FORWARD_MOVE_DELAY = 6000;
const int BACKWARD_MOVE_SPEED = 50;
const int BACKWARD_MOVE_DELAY = 6000;
const int TURN_SPEED = 150;
const int TURN_DELAY = 800;
const int PWM_FREQ = 500; // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8;
Adafruit_MotorShield motorShield = Adafruit_MotorShield(MOTOR_SHIELD_ADDRESS);
Adafruit_DCMotor *motor1 = motorShield.getMotor(1);
Adafruit_DCMotor *motor2 = motorShield.getMotor(2);
Adafruit_DCMotor *motor3 = motorShield.getMotor(3);
Adafruit_DCMotor *motor4 = motorShield.getMotor(4);

/*
 * Movement functions
 */
// Forward
void moveForward()
{
  motor1->run(FORWARD);
  motor1->setSpeed(FORWARD_MOVE_SPEED);
  motor2->run(BACKWARD);
  motor2->setSpeed(FORWARD_MOVE_SPEED);
  motor3->run(BACKWARD);
  motor3->setSpeed(FORWARD_MOVE_SPEED);
  motor4->run(FORWARD);
  motor4->setSpeed(FORWARD_MOVE_SPEED);
}
// Backward
void moveBackward()
{
  motor1->run(BACKWARD);
  motor1->setSpeed(FORWARD_MOVE_SPEED);
  motor2->run(FORWARD);
  motor2->setSpeed(FORWARD_MOVE_SPEED);
  motor3->run(FORWARD);
  motor3->setSpeed(FORWARD_MOVE_SPEED);
  motor4->run(BACKWARD);
  motor4->setSpeed(FORWARD_MOVE_SPEED);
}
// Left turn
void turnLeft()
{
  motor1->run(FORWARD);
  motor1->setSpeed(TURN_SPEED);
  motor2->run(BACKWARD);
  motor2->setSpeed(TURN_SPEED);
  motor3->run(FORWARD);
  motor3->setSpeed(TURN_SPEED);
  motor4->run(BACKWARD);
  motor4->setSpeed(TURN_SPEED);
}
// Right turn
void turnRight()
{
  motor1->run(BACKWARD);
  motor1->setSpeed(TURN_SPEED);
  motor2->run(FORWARD);
  motor2->setSpeed(TURN_SPEED);
  motor3->run(BACKWARD);
  motor3->setSpeed(TURN_SPEED);
  motor4->run(FORWARD);
  motor4->setSpeed(TURN_SPEED);
}
// Stop
void stopMove()
{
  motor1->run(RELEASE);
  motor2->run(RELEASE);
  motor3->run(RELEASE);
  motor4->run(RELEASE);
}

/*
 * Movement step functions
 */
// Forward step
void stepForward()
{
  moveForward();
  delay(FORWARD_MOVE_DELAY);
  stopMove();
}
// Backward step
void stepBackward()
{
  moveBackward();
  delay(BACKWARD_MOVE_DELAY);
  stopMove();
}
// Left turn step
void stepLeft()
{
  turnLeft();
  delay(TURN_DELAY);
  stopMove();
}
// Right turn step
void stepRight()
{
  turnRight();
  delay(TURN_DELAY);
  stopMove();
}

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

  // Motor shield initialization
  if (!motorShield.begin())
  {
    Serial.println("Could not find Motor Shield!");
    while (1)
      ;
  }

  // OLED init and check
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    printMessage("OLED allocation failed!");
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