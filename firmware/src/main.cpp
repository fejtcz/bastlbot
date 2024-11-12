#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MotorShield.h>

// PINs definitions
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_BUZZER 27
#define PIN_FRONT_LED 25
#define PIN_REAR_LED 26
#define PIN_TRACE_LEFT 32
#define PIN_TRACE_RIGHT 33

// PWM definitions
const int PWM_FREQ = 500; // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8;

// OLED display definitions
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// Motor Shield and motors definitions
#define MOTOR_SHIELD_ADDRESS 0x40
const int FORWARD_MOVE_SPEED = 80;
const int FORWARD_MOVE_DELAY = 1150;
const int BACKWARD_MOVE_SPEED = 80;
const int BACKWARD_MOVE_DELAY = 1150;
const int TURN_SPEED = 150;
const int TURN_DELAY = 800;
Adafruit_MotorShield motorShield = Adafruit_MotorShield(MOTOR_SHIELD_ADDRESS);
Adafruit_DCMotor *motor1 = motorShield.getMotor(1);
Adafruit_DCMotor *motor2 = motorShield.getMotor(2);
Adafruit_DCMotor *motor3 = motorShield.getMotor(3);
Adafruit_DCMotor *motor4 = motorShield.getMotor(4);

// WiFi and Webserver definitions
#define WIFI_SSID "BastlBot"
#define WIFI_PASSWORD "B@stlbot24"
IPAddress local_ip(10, 0, 0, 1);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer webserver(80); // Webserver on port 80

// Variable for line following
bool traceLine = false;
const int TRACE_TRASHOLD = 1000;

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
 * Set the LED brightness
 * @param channel - PWM channel
 * @param dutyCycle - duty cycle
 */
void setLEDBrightness(int channel, int dutyCycle)
{
  ledcWrite(channel, dutyCycle);
}

/*
 * Blink the LED with fade effect
 * @param channel - PWM channel
 * @param delayTime - delay time
 * @param maxDutyCycle - maximum duty cycle
 */
void blinkLED(int channel, int delayTime, int maxDutyCycle)
{
  for (int dutyCycle = 0; dutyCycle <= maxDutyCycle; dutyCycle++)
  {
    setLEDBrightness(channel, dutyCycle);
    delay(delayTime);
  }

  for (int dutyCycle = maxDutyCycle; dutyCycle >= 0; dutyCycle--)
  {
    setLEDBrightness(channel, dutyCycle);
    delay(delayTime);
  }
}

/*
 * Play startup sound
 */
void playStartupSound()
{
  beep(1000, 150);
  delay(50);
  beep(2000, 150);
  delay(50);
  beep(3000, 150);
  delay(50);
  beep(2000, 150);
}

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
  motor1->setSpeed(BACKWARD_MOVE_SPEED);
  motor2->run(FORWARD);
  motor2->setSpeed(BACKWARD_MOVE_SPEED);
  motor3->run(FORWARD);
  motor3->setSpeed(BACKWARD_MOVE_SPEED);
  motor4->run(BACKWARD);
  motor4->setSpeed(BACKWARD_MOVE_SPEED);
}

// Left turn
void turnLeft()
{
  motor1->run(FORWARD);
  motor1->setSpeed(TURN_SPEED);
  motor2->run(BACKWARD);
  motor2->setSpeed(TURN_SPEED);
  motor3->run(FORWARD);
  motor3->setSpeed(TURN_SPEED - 50);
  motor4->run(BACKWARD);
  motor4->setSpeed(TURN_SPEED - 50);
}

// Right turn
void turnRight()
{
  motor1->run(BACKWARD);
  motor1->setSpeed(TURN_SPEED);
  motor2->run(FORWARD);
  motor2->setSpeed(TURN_SPEED);
  motor3->run(BACKWARD);
  motor3->setSpeed(TURN_SPEED - 50);
  motor4->run(FORWARD);
  motor4->setSpeed(TURN_SPEED - 50);
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
 * Step functions
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
 * Run instructions
 * @param instruction - instructions to be run
 */
void runInstructions(String instructions)
{
  printMessage("Running instructions: " + String(instructions));

  // Turn on the both LEDs
  setLEDBrightness(0, 255);
  setLEDBrightness(1, 80); // Lower brightness for the rear LED

  // Convert the instruction string to an array of characters
  int length = instructions.length();
  char instructionsArray[length + 1];
  instructions.toCharArray(instructionsArray, length + 1);

  // Run the instructions
  for (int i = 0; i < length; i++)
  {
    switch (instructionsArray[i])
    {
    case '1':
      stepForward();
      break;
    case '2':
      stepLeft();
      break;
    case '3':
      stepRight();
      break;
    case '4':
      stepBackward();
      break;
    default:
      break;
    }
  }

  // Turn off the front LEDs
  setLEDBrightness(0, 0);
  // Full on the rear LED
  setLEDBrightness(1, 255);
  delay(2000);
}

/*
 * Webserver Homepage
 */
void webserverHomepage()
{
  String content = "<html>";
  content += "<head>";
  content += "<meta charset=\"UTF-8\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<title>BastlBot - Stepper</title>";
  content += "<style type=\"text/css\">";
  content += "body {background-color: #f0f0f0;font-family: Arial, sans-serif;text-align: center;}";
  content += "input {width: 80%;height: 50px;font-size: 2em;font-weight: bold;margin: 20px;text-align: center;}";
  content += "table {margin: 20 auto;width: 50%;text-align: center;}";
  content += "table button {width: 100px;height: 100px;margin: 10px;font-size: 2em;}";
  content += "table button.middleone {width: 200px;}";
  content += "table button.bigone {width: 100%;}";
  content += "table button.red {background-color: red;color: white;}";
  content += "table button.green {background-color: green;color: white;}";
  content += "</style>";
  content += "<script>";
  content += "let instructions = [];";
  content += "function addInstruction(instruction, arrow) {instructions.push(instruction);document.querySelector('input[name=\"input\"]').value += ' ' + arrow + ' ';}";
  content += "function clearInstructions() {instructions = [];document.querySelector('input[name=\"input\"]').value = '';}";
  content += "function runInstructions() {instructionsString = instructions.join('');window.location.href = '/run?instructions=' + instructionsString;}";
  content += "</script>";
  content += "</head>";
  content += "<body>";
  content += "<h1>BastlBot</h1>";
  content += "<h2>Stepper</h2>";
  content += "<input type=\"text\" name=\"input\" disabled>";
  content += "<table>";
  content += "<tr>";
  content += "<td></td>";
  content += "<td><button onclick=\"addInstruction(1, '&uarr;')\">&uarr;</button></td>";
  content += "<td></td>";
  content += "</tr>";
  content += "<tr>";
  content += "<td><button onclick=\"addInstruction(2, '&larr;')\">&larr;</button></td>";
  content += "<td><button class=\"red\" onclick=\"clearInstructions()\">X</button></td>";
  content += "<td><button onclick=\"addInstruction(3, '&rarr;')\">&rarr;</button></td>";
  content += "</tr>";
  content += "<tr>";
  content += "<td></td>";
  content += "<td><button onclick=\"addInstruction(4, '&darr;')\">&darr;</button></td>";
  content += "<td></td>";
  content += "</tr>";
  content += "<tr>";
  content += "<td colspan=\"3\">";
  content += "<button class=\"middleone green\" onclick=\"runInstructions()\">GO</button>";
  content += "</td>";
  content += "</tr>";
  content += "<tr>";
  content += "<td colspan=\"3\">";
  content += "<button class=\"bigone\" onclick=\"window.location.href = '/traceline'\">TRACE LINE</button>";
  content += "</td>";
  content += "</tr>";
  content += "</table>";
  content += "</body>";
  content += "</html>";
  webserver.send(200, "text/html", content);
}

/*
 * Webserver Trace Line
 */
void webserverTraceline()
{
  traceLine = true;

  String content = "<html>";
  content += "<head>";
  content += "<meta charset=\"UTF-8\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<title>BastlBot - Trace Line</title>";
  content += "<style type=\"text/css\">";
  content += "body {background-color: #f0f0f0;font-family: Arial, sans-serif;text-align: center;}";
  content += "table {margin: 20 auto;width: 50%;text-align: center;}";
  content += "table button {width: 100px;height: 100px;margin: 10px;font-size: 2em;}";
  content += "table button.middleone {width: 200px;}";
  content += "table button.bigone {width: 100%;}";
  content += "table button.red {background-color: red;color: white;}";
  content += "table button.green {background-color: green;color: white;}";
  content += "</style>";
  content += "</head>";
  content += "<body>";
  content += "<h1>BastlBot</h1>";
  content += "<h2>Trace Line</h2>";
  content += "<table>";
  content += "<tr>";
  content += "<td colspan=\"3\">";
  content += "<button class=\"bigone red\" onclick=\"window.location.href = '/stop'\">STOP</button>";
  content += "</td>";
  content += "</tr>";
  content += "</table>";
  content += "</body>";
  content += "</html>";
  webserver.send(200, "text/html", content);
}

/*
 * Webserver Stop
 */
void webserverStop()
{
  traceLine = false;
  stopMove();
  webserver.sendHeader("Location", "/");
  webserver.send(303);
}

/*
 * Webserver Run
 */
void webserverRun()
{
  String instructions = webserver.arg("instructions");
  if (instructions != "")
  {
    runInstructions(instructions);
  }
  webserver.sendHeader("Location", "/");
  webserver.send(303);
}

/*
 * Webserver Not Found
 */
void webserverNotFound()
{
  webserver.send(404, "text/html", "<center><h1>HTTP 404</h1><h2>Not found!</h2></center>");
}

/*
 * Main Setup function
 */
void setup()
{
  // Serial output initialization
  Serial.begin(115200);
  printMessage("***** BastlBot *****");
  printMessage("BastlBot is starting...");

  // LED initialization
  ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PIN_REAR_LED, 1);
  ledcAttachPin(PIN_FRONT_LED, 0);

  // OLED initialization and check
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    printMessage("OLED allocation failed");
    return;
  }

  // Initialize the Trace line pins
  pinMode(PIN_TRACE_LEFT, INPUT);
  pinMode(PIN_TRACE_RIGHT, INPUT);

  // Motor shield initialization
  if (!motorShield.begin())
  {
    Serial.println("Could not find Motor Shield!.");
    while (1)
      ;
  }

  // OLED Startup message
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, 10);
  display.println("BastlBot");
  display.setTextSize(1);
  display.setCursor(10, 45);
  display.println("IP: 10.0.0.1");
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.println("Bat. voltage: 0.00V");
  display.display();

  // WiFi initialization
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  printMessage("WiFi AP started");

  // Webserver route definitions
  webserver.on("/", webserverHomepage);
  webserver.on("/run", webserverRun);
  webserver.on("/traceline", webserverTraceline);
  webserver.on("/stop", webserverStop);
  webserver.onNotFound(webserverNotFound);

  // Webserver start
  webserver.begin();
  Serial.println("HTTP server started");

  // Play startup sound
  playStartupSound();

  // Turn on the front and rear LED
  setLEDBrightness(0, 255);
  setLEDBrightness(1, 255);
}

/*
 * Main Loop function
 */
void loop()
{
  if (traceLine)
  {
    // Get the trace sensor values
    int traceLeft = analogRead(PIN_TRACE_LEFT);
    int traceRight = analogRead(PIN_TRACE_RIGHT);

    // Print the trace sensor values
    printMessage("Trace left: " + String(traceLeft) + ", Trace right: " + String(traceRight));

    // Line following logic
    if (traceLeft > TRACE_TRASHOLD && traceRight > TRACE_TRASHOLD)
    {
      stopMove();
    }
    else if (traceLeft > TRACE_TRASHOLD)
    {
      turnRight();
    }
    else if (traceRight > TRACE_TRASHOLD)
    {
      turnLeft();
    }
    else
    {
      moveForward();
    }
  }

  webserver.handleClient();
}