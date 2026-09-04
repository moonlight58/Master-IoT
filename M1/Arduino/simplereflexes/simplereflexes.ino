#include <ESP8266WiFi.h>

#define PIN_LED D1
#define PIN_BUTTON D2

int blink = 0;  // 0=blink, 1= button
int state = 0;
int nbBlinks;
volatile int countPush;  // incremented by testSwitch()
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 50;
long t = millis();
unsigned long t1 = 0, t2 = 0;

void ICACHE_RAM_ATTR testSwitch() {
  unsigned long now = millis();
  if (now - lastInterruptTime > debounceDelay) {
    countPush++;
    lastInterruptTime = now;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // power off Wifi to save energy
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);

  pinMode(PIN_BUTTON, INPUT_PULLUP);  // switch
  pinMode(PIN_LED, OUTPUT);    // LED
  digitalWrite(PIN_LED, LOW);  // switch off the led at start

  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), testSwitch, FALLING);

  randomSeed(analogRead(0));
  delay(10);
}

void loop() {
  if (state == 0) {
    // wait 5 secs
    delay(5000);

    // assign nbBlinks
    nbBlinks = random(2, 10);

    // do the blinks
    for (int i = 1; i < nbBlinks; i++) {
      digitalWrite(PIN_LED, HIGH);
      delay(500);
      digitalWrite(PIN_LED, LOW);
      delay(500);
    }

    //last blink (1sec)
    digitalWrite(PIN_LED, HIGH);
    delay(1000);
    digitalWrite(PIN_LED, LOW);

    countPush = 0;

    // t1 = current time;
    t1 = millis();
    Serial.printf("t1 = %d\n", t1);
    state = 1;
  } else if (state == 1) {
    if (countPush >= nbBlinks) {  // see Rk below for the >=
      // t2 =  current time
      t2 = millis();
      // print t2-t1
      Serial.printf("t2 = %lu\n", t2);
      Serial.printf("final time (t2-t1): %lu ms\n", t2 - t1);
      
      digitalWrite(PIN_LED, HIGH);
      delay(200);
      digitalWrite(PIN_LED, LOW);

      state = 0;
    }
  }
}
