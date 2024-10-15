#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_MotorShield.h>

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

// Define WiFi credentials and server PORT
#define WIFI_SSID "SSID"
#define WIFI_PASWORD "PASWORD"
WebServer webserver(80);

// Define motor shield and motors
Adafruit_MotorShield motorShield = Adafruit_MotorShield(0x40);
Adafruit_DCMotor *motor1 = motorShield.getMotor(1);
Adafruit_DCMotor *motor2 = motorShield.getMotor(2);
Adafruit_DCMotor *motor3 = motorShield.getMotor(3);
Adafruit_DCMotor *motor4 = motorShield.getMotor(4);

// Display icons
// UP
const unsigned char upIcon[] PROGMEM = {
    0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xfc, 0x3f,
    0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};
// DOWN
const unsigned char downIcon[] PROGMEM = {
    0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f,
    0xfc, 0x3f, 0xe0, 0x07, 0xf0, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};
// LEFT
const unsigned char leftIcon[] PROGMEM = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xf9, 0xff, 0xf1, 0xff, 0xe0, 0x07, 0xc0, 0x03,
    0xc0, 0x03, 0xe0, 0x07, 0xf1, 0xff, 0xf9, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
// RIGHT
const unsigned char rightIcon[] PROGMEM = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0x9f, 0xff, 0x8f, 0xe0, 0x07, 0xc0, 0x03,
    0xc0, 0x03, 0xe0, 0x07, 0xff, 0x8f, 0xff, 0x9f, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// Movement functions
// Move forward
void moveForward()
{
  motor1->run(FORWARD);
  motor1->setSpeed(100);
  motor2->run(BACKWARD);
  motor2->setSpeed(100);
  motor3->run(BACKWARD);
  motor3->setSpeed(100);
  motor4->run(FORWARD);
  motor4->setSpeed(100);
}

// Move backward
void moveBackward()
{
  motor1->run(BACKWARD);
  motor1->setSpeed(100);
  motor2->run(FORWARD);
  motor2->setSpeed(100);
  motor3->run(FORWARD);
  motor3->setSpeed(100);
  motor4->run(BACKWARD);
  motor4->setSpeed(100);
}

// Left turn
void leftTurn()
{
  motor1->run(FORWARD);
  motor1->setSpeed(100);
  motor2->run(BACKWARD);
  motor2->setSpeed(100);
  motor3->run(FORWARD);
  motor3->setSpeed(100);
  motor4->run(BACKWARD);
  motor4->setSpeed(100);
}

// Right turn
void rightTurn()
{
  motor1->run(BACKWARD);
  motor1->setSpeed(100);
  motor2->run(FORWARD);
  motor2->setSpeed(100);
  motor3->run(BACKWARD);
  motor3->setSpeed(100);
  motor4->run(FORWARD);
  motor4->setSpeed(100);
}

// Stop movement
void stopMovement()
{
  motor1->run(RELEASE);
  motor2->run(RELEASE);
  motor3->run(RELEASE);
  motor4->run(RELEASE);
}

// Forward step
void forwardStep()
{
  moveForward();
  delay(1000);
  stopMovement();
}

// Backward step
void backwardStep()
{
  moveBackward();
  delay(1000);
  stopMovement();
}

// Left step
void leftStep()
{
  leftTurn();
  delay(1200);
  stopMovement();
}

// Right step
void rightStep()
{
  rightTurn();
  delay(1200);
  stopMovement();
}

// List of step instructions, stored in an array
int instructions[10];
int instList = 0;

/*
 * Main page of webserver
 */
void mainPage()
{
  String content = "<html>";
  content += "<head>";
  content += "<meta charset=\"UTF-8\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<title>BastlBot</title>";
  content += "<style type=\"text/css\">";
  content += "body {font-family: Arial, sans-serif;margin: 0;padding: 0;background-color: #f0f0f0;text-align: center;}";
  content += "h1 {text-align: center;margin-top: 50px;color: #333;}";
  content += "a {display: inline-block;margin: 10px;width: 200px;padding: 10px 20px;background-color: #333;color: #fff;text-decoration: none;border-radius: 5px;}";
  content += "a.green {background-color: #4CAF50;}";
  content += "a.red {background-color: #f44336;}";
  content += "</style>";
  content += "</head>";
  content += "<body>";
  content += "<h1>BastlBot</h1>";
  content += "<p>Řízení krokového pohybu</p>";
  content += "<a href=\"/left\">LEFT</a>";
  content += "<a href=\"/forward\">FORWARD</a>";
  content += "<a href=\"/right\">RIGHT</a>";
  content += "<br><a class=\"green\" href=\"/run\">GO!</a>";
  content += "<br><a class=\"red\" href=\"/clear\">RESET</a>";
  content += "</body>";
  content += "</html>";
  webserver.send(200, "text/html", content);
}

/*
 * Redirect to the home page
 */
void redirectToHome()
{
  webserver.sendHeader("Location", "/");
  webserver.send(303);
}

/*
 * Function for Display update
 * @param current - current instruction
 */
void updateDisplay(int current)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(current);
  for (int i = 0; i < (current + 1); i++)
  {
    int x = (i < 5) ? ((20 * i) + 16) : ((20 * i) - 84);
    int y = (i < 5) ? 16 : 40;

    switch (instructions[i])
    {
    case 1:
      display.drawBitmap(x, y, leftIcon, 16, 16, WHITE);
      break;
    case 2:
      display.drawBitmap(x, y, rightIcon, 16, 16, WHITE);
      break;
    case 3:
      display.drawBitmap(x, y, upIcon, 16, 16, WHITE);
      break;
    default:
      break;
    }
    display.display();
  }
}

/*
 * Functions for display clear
 */
void clearDisplay()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 30);
  display.println("Zadej instrukce");
  display.display();
}

/*
 * Function for instruction collection
 * @param instruction - instruction to be added
 */
void addInstruction(int instruction)
{
  instructions[instList] = instruction;
  instList++;
  updateDisplay(instList);
}

/*
 * Function for running the instructions
 */
void runInstructions()
{
  for (int i = 0; i <= (instList - 1); i++)
  {
    switch (instructions[i])
    {
    case 1:
      leftStep();
      break;
    case 2:
      rightStep();
      break;
    case 3:
      forwardStep();
      break;
    }
  }
}

/*
 * Function for reseting the instructions
 */
void clearInstructions()
{
  instList = 0;
  clearDisplay();
}

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

  // Startup sound
  startupSound();

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
  display.display();

  // Motor shield initialization
  if (!motorShield.begin())
  {
    Serial.println("Could not find Motor Shield!.");
    while (1)
      ;
  }

  // WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi network with IP: " + WiFi.localIP().toString());
  beep(2000, 500); // Confirm beep

  // Show additional information (WiFi and battery)
  display.setCursor(10, 45);
  display.println("IP: " + WiFi.localIP().toString());
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.println("Bat. voltage: " + String(voltageValue) + "V");
  display.display();

  // Activate MDNS
  if (MDNS.begin("espwebserver"))
  {
    Serial.println("MDNS responder is started");
  }

  // Server routes
  webserver.on("/", mainPage);
  webserver.on("/left", []()
               {
                  addInstruction(1);
                  Serial.println("LEFT");
                  redirectToHome(); });

  webserver.on("/right", []()
               {
                  addInstruction(2);
                  Serial.println("RIGHT");
                  redirectToHome(); });
  webserver.on("/forward", []()
               {
                  addInstruction(3);
                  Serial.println("FORWARD");
                  redirectToHome(); });
  webserver.on("/clear", []()
               {
                  clearInstructions();
                  Serial.println("CLEAR");
                  redirectToHome(); });

  webserver.on("/run", []()
               {
                  runInstructions();
                  Serial.println("RUN");
                  clearInstructions();
                  redirectToHome(); });

  // Webserver start
  webserver.begin();
}

void loop()
{
  // WiFi Client detection and handling
  webserver.handleClient();
  delay(10);
}