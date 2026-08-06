#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// ================= OLED Display =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= Servo =================
Servo gateServo;
int servoPin = 18;
int openPos = 90;
int closePos = 0;

// ================= IR Sensors =================
int irEntrance = 19;
int irInside   = 23;
int irExit     = 25;

// ================= Parking =================
int totalSlots = 3;
int maxSlots   = 3;
bool entryInProgress = false;
int lastExitState = HIGH;

// ======== Function prototypes ========
void openGate();
void closeGate();
void displayMessage(String msg, int slots, String extra = "");
void displaySlots();

// ================= Setup =================
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.print("Slots: ");
  display.println(totalSlots);
  display.display();

  gateServo.attach(servoPin, 500, 2400);
  gateServo.write(closePos);

  pinMode(irEntrance, INPUT);
  pinMode(irInside, INPUT);
  pinMode(irExit, INPUT);
}

// ================= Loop =================
void loop() {
  int entranceState = digitalRead(irEntrance);
  int insideState   = digitalRead(irInside);
  int exitState     = digitalRead(irExit);

  if (!entryInProgress && entranceState == LOW) {
    if (totalSlots > 0) {
      entryInProgress = true;
      openGate();
      displayMessage("WELCOME!", totalSlots);
    } else {
      displayMessage("SORRY!", totalSlots, "No Slots");
      closeGate();
      delay(1500);
    }
  }

  if (entryInProgress && insideState == LOW) {
    if (totalSlots > 0) totalSlots--;
    closeGate();
    entryInProgress = false;
    displaySlots();
  }

  if (lastExitState == HIGH && exitState == LOW) {
    if (totalSlots < maxSlots) totalSlots++;
    displayMessage("EXITING...", totalSlots);
    delay(1000);
    displaySlots();
  }

  lastExitState = exitState;
}

// ================= Helper Functions =================
void openGate() {
  gateServo.write(openPos);
  delay(1000);
}

void closeGate() {
  gateServo.write(closePos);
  delay(1000);
}

void displayMessage(String msg, int slots, String extra) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(msg);

  if (extra != "") {
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.println(extra);
  } else {
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("Slots: ");
    display.println(slots);
  }

  display.display();
}

void displaySlots() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Slots: ");
  display.println(totalSlots);
  display.display();
}
