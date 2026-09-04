#include <ESP8266WiFi.h>

volatile bool buttonWasPressed = false;  
volatile bool toggleRequested   = false;

bool ledState = false; // current state of the LED

// Interrupt handler: must be fast, no Serial calls, must be in IRAM
ICACHE_RAM_ATTR void handleButtonChange() {
  int state = digitalRead(D2);

  if (state == HIGH) {
    buttonWasPressed = true;
  } else {
    if (buttonWasPressed) {
      toggleRequested = true;
      buttonWasPressed = false;
    }
  }
}

void testSwitch() {
  ledState = !ledState;
  digitalWrite(D1, ledState ? HIGH : LOW);
  Serial.println(ledState ? "light on" : "light off");
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // power off Wifi to save energy
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);

  pinMode(D2, INPUT); // switch
  pinMode(D1, OUTPUT); // LED
  digitalWrite(D1, LOW); // switch off the led at start
  delay(10);

  attachInterrupt(digitalPinToInterrupt(D2), handleButtonChange, CHANGE);
}

void loop() {
  if (toggleRequested) {
    toggleRequested = false;
    testSwitch();
  }
}