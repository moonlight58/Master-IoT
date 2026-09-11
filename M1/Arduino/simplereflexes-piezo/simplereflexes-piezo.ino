#include <ESP8266WiFi.h>
#include <IRremote.hpp>

#define PIN_LED         D1
#define PIN_PIEZO       A0
#define THRESHOLD       30

int state = 0;       // 0 = wait+blink phase, 1 = waiting for the piezo mashing
int nbBlinks;
unsigned long t1 = 0, t2 = 0;
int countPush = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // power off Wifi to save energy
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  // LED off at boot

  randomSeed(analogRead(A0));
  delay(10);
}

void loop() {
  if (state == 0) {
    // wait 5 secs before blinking
    delay(5000);

    nbBlinks = random(2, 10);  // 2..9
    Serial.printf("nbBlinks = %d\n", nbBlinks);

    for (int i = 1; i < nbBlinks; i++) {
      digitalWrite(PIN_LED, HIGH);
      delay(500);
      digitalWrite(PIN_LED, LOW);
      delay(500);
    }

    digitalWrite(PIN_LED, HIGH);
    delay(1000);
    digitalWrite(PIN_LED, LOW);

    t1 = millis();
    Serial.printf("t1 = %lu\n", t1);
    state = 1;

  } else if (state == 1) {

    int PIEZOVAL = analogRead(PIN_PIEZO);

    if(PIEZOVAL > THRESHOLD) {
      Serial.println("Piezo pushed");
      countPush++;
    }

    if (countPush >= nbBlinks) {
      t2 = millis();
      Serial.printf("t2 = %lu\n", t2);
      Serial.printf("final time (t2-t1): %lu ms\n", t2 - t1);

      digitalWrite(PIN_LED, HIGH);
      delay(200);
      digitalWrite(PIN_LED, LOW);

      state = 0;
    }
    
  }
}
