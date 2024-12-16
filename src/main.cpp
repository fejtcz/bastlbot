#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
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

// WiFi and Webserver definitions
#define WIFI_SSID "BastlBot"
#define WIFI_PASSWORD "B@stlbot24"
IPAddress local_ip(10, 0, 0, 1);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer webserver(80); // Webserver on port 80

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

/*
 * Display messages
 */
// Startup message
void displayStartupMessage()
{
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, 10);
  display.println("BastlBot");
  display.setTextSize(1);
  display.setCursor(40, 30);
  display.println("(Stepper)");
  display.setCursor(10, 45);
  display.println("IP Addr: 10.0.0.1");
  display.setCursor(10, 55);
  display.println("Bat. voltage: 0.00V"); // TODO - dodelat mereni napeti baterie
  display.display();
}
// Waiting message
void displayWaitingMessage()
{
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(5, 10);
  display.println("Waiting...");
  display.display();
}
// Forward arrow
void displayForwardArrow()
{
  display.setRotation(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(8);
  display.setCursor(10, 35);
  display.println(">");
  display.display();
}
// Backward arrow
void displayBackwardArrow()
{
  display.setRotation(3);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(8);
  display.setCursor(10, 35);
  display.println(">");
  display.display();
}
// Left arrow
void displayLeftArrow()
{
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(8);
  display.setCursor(30, 5);
  display.println("<");
  display.display();
}
// Right arrow
void displayRightArrow()
{
  display.setRotation(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(8);
  display.setCursor(40, 5);
  display.println(">");
  display.display();
}

/*
 * Run step instructions
 * @param instructions - instructions to be run
 */
void runInstructions(String instructions)
{
  printMessage("Running instructions: " + String(instructions));

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
      displayForwardArrow();
      stepForward();
      break;
    case '2':
      displayLeftArrow();
      stepLeft();
      break;
    case '3':
      displayRightArrow();
      stepRight();
      break;
    case '4':
      displayBackwardArrow();
      stepBackward();
      break;
    default:
      break;
    }
  }
}

/*
 * Webserver pages
 */
// Homepage
void webserverHomepage()
{
  String content = "<html>";
  content += "<head>";
  content += "<meta charset=\"UTF-8\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<title>BastlBot</title>";
  content += "<style type=\"text/css\">";
  content += "body {background-color: #f0f0f0;font-family: Arial, sans-serif;text-align: center;}";
  content += "input {width: 80%;height: 50px;font-size: 2em;font-weight: bold;margin: 20px;text-align: center;}";
  content += "table {margin: 20 auto;width: 50%;text-align: center;}";
  content += "table button {width: 100px;height: 100px;margin: 10px;font-size: 2em;}";
  content += "table button.middleone {width: 200px;}";
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
  content += "<input type=\"text\" name=\"input\" value=\"\" disabled>";
  content += "<table>";
  content += "<tr><td></td><td><button onclick=\"addInstruction(1, '&uarr;')\">&uarr;</button></td><td></td></tr>";
  content += "<tr><td><button onclick=\"addInstruction(2, '&larr;')\">&larr;</button></td><td><button class=\"red\" onclick=\"clearInstructions()\">X</button></td><td><button onclick=\"addInstruction(3, '&rarr;')\">&rarr;</button></td></tr>";
  content += "<tr><td></td><td><button onclick=\"addInstruction(4, '&darr;')\">&darr;</button></td><td></td></tr>";
  content += "<tr><td colspan=\"3\"><button class=\"green middleone\" onclick=\"runInstructions()\">GO</button></td></tr>";
  content += "</table>";
  content += "</body>";
  content += "</html>";
  webserver.send(200, "text/html", content);
  displayWaitingMessage();
}
// Run instructions
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
// Not found page
void webserverNotFound()
{
  webserver.send(404, "text/html", "<center><h1>HTTP 404</h1><h2>Not found!</h2></center>");
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

  // WiFi initialization
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  printMessage("WiFi AP started...");

  // Webserver route definitions
  webserver.on("/", webserverHomepage);
  webserver.on("/run", webserverRun);
  webserver.onNotFound(webserverNotFound);

  // Webserver start
  webserver.begin();
  Serial.println("HTTP server started...");

  // OLED Startup message
  displayStartupMessage();

  // Play startup sound
  playStartupSound();
}

void loop()
{
  webserver.handleClient();
}