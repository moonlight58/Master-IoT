#include <ESP8266WiFi.h>
#include <IRremote.hpp>

#define PIN_LED         D1
#define PIN_IR_REMOTE   D3   // IR receiver OUT pin

unsigned long buttonCode[10] = {
  0x0,  
  0x0,  // 0x45,   // should not be used since: 2 <= nbBlink >= 9
  0x0,  // 0x46,   // should not be used since: 2 <= nbBlink >= 9 
  0xB847FF00, 
  0xBB44FF00, 
  0xBF40FF00, 
  0xBC43FF00, 
  0xF807FF00, 
  0xEA15FF00, 
  0xF609FF00
};
// --------------------------------------------------------------------

int state = 0;       // 0 = wait+blink phase, 1 = waiting for the right button
int nbBlinks;
unsigned long t1 = 0, t2 = 0;

// returns the digit (2..9) corresponding to a received IR code, or -1 if unknown
int codeToDigit(unsigned long code) {
  for (int d = 2; d <= 9; d++) {
    if (buttonCode[d] == code) {
      return d;
    }
  }
  return -1;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // power off Wifi to save energy
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);  // LED off at boot

  IrReceiver.begin(PIN_IR_REMOTE, ENABLE_LED_FEEDBACK);

  randomSeed(analogRead(A0));
  delay(10);
}

void loop() {
  if (state == 0) {
    // wait 5 secs before blinking
    delay(5000);

    // choose how many times to blink
    nbBlinks = random(2, 10);  // 2..9
    Serial.printf("nbBlinks = %d\n", nbBlinks);

    // all blinks except the last one: 500ms ON
    for (int i = 1; i < nbBlinks; i++) {
      digitalWrite(PIN_LED, HIGH);
      delay(500);
      digitalWrite(PIN_LED, LOW);
      delay(500);
    }

    // last blink: 1 sec ON, so the user knows the sequence ended
    digitalWrite(PIN_LED, HIGH);
    delay(1000);
    digitalWrite(PIN_LED, LOW);

    // discard any IR signal that might have arrived during the blinking
    while (IrReceiver.decode()) {
      IrReceiver.resume();
    }

    t1 = millis();
    Serial.printf("t1 = %lu\n", t1);
    state = 1;

  } else if (state == 1) {
    if (IrReceiver.decode()) {
      unsigned long code = IrReceiver.decodedIRData.decodedRawData;
      int digit = codeToDigit(code);

      if (digit != -1) {
        Serial.printf("Button pressed: %d\n", digit);
      } else {
        Serial.println("Unknown / repeat code ignored");
      }

      if (digit == nbBlinks) {
        t2 = millis();
        Serial.printf("t2 = %lu\n", t2);
        Serial.printf("final time (t2-t1): %lu ms\n", t2 - t1);

        // warn the user the sequence is over
        digitalWrite(PIN_LED, HIGH);
        delay(200);
        digitalWrite(PIN_LED, LOW);

        state = 0;
      }
      // wrong button (or unknown code): just keep waiting, time keeps running

      IrReceiver.resume();  // ready to receive the next code
    }
  }
}
