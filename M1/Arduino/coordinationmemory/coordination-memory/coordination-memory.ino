#include <ESP8266WiFi.h>
#include <IRremote.hpp>

#define PIN_LED         D1
#define PIN_BUTTON      D2   // wired with INPUT_PULLUP, button pulls to GND
#define PIN_PIEZO       A0   // piezo disc across a resistor to GND
#define PIN_IR_RECEIVE  D3   // IR receiver OUT pin

// CHANGED: Use uint32_t instead of uint16_t to match 32-bit IR codes
const uint32_t digitCommand[10] = {
  0xE619FF00,   // 0
  0xBA45FF00,   // 1
  0xB946FF00,   // 2
  0xB847FF00,   // 3
  0xBB44FF00,   // 4
  0xBF40FF00,   // 5
  0xBC43FF00,   // 6
  0xF807FF00,   // 7
  0xEA15FF00,   // 8
  0xF609FF00    // 9
};

// CHANGED: Parameter must accept uint32_t
int commandToDigit(uint32_t command) {
  for (int d = 0; d <= 9; d++) {
    if (digitCommand[d] == command) return d;
  }
  return -1;
}

// Application states 
enum AppState { STATE_INIT, STATE_MODE_BUTTON_FIRST, STATE_MODE_PIEZO_FIRST, STATE_MODE_IR };
AppState state = STATE_INIT;

int nbBlinks1, nbBlinks2, nbBlinks3;

// targets / progress for button+piezo modes
int targetButton, targetPiezo;
int doneButton, donePiezo;
bool expectingButton; 

// targets / progress for IR mode
char expectedDigits[4]; // up to 2 digits + null terminator
int expectedLen;
int digitIndex;

unsigned long t1 = 0, t2 = 0;

// Button handling (interrupt + debounce) 
volatile bool buttonEventFlag = false;
volatile unsigned long lastButtonInterrupt = 0;

void IRAM_ATTR onButtonPressed() {
  unsigned long now = millis();
  if (now - lastButtonInterrupt > 50) {   // debounce
    buttonEventFlag = true;
  }
  lastButtonInterrupt = now;
}

bool checkButtonClickOnce() {
  if (buttonEventFlag) {
    buttonEventFlag = false;
    return true;
  }
  return false;
}

// Piezo handling (threshold + debounce) 
const int PIEZO_THRESHOLD = 100;     
const unsigned long PIEZO_REFRACTORY = 100; 
bool piezoArmed = true;
unsigned long lastPiezoHit = 0;

bool checkPiezoHitOnce() {
  int v = analogRead(PIN_PIEZO);
  unsigned long now = millis();

  if (v > PIEZO_THRESHOLD && piezoArmed && (now - lastPiezoHit > PIEZO_REFRACTORY)) {
    piezoArmed = false;
    lastPiezoHit = now;
    return true;
  }
  if (v < PIEZO_THRESHOLD / 2) {  
    piezoArmed = true;
  }
  return false;
}

// Blink helpers
void blinkSeries300(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(300);
    digitalWrite(PIN_LED, LOW);
    delay(300);
  }
}

int blinkFinalSeries500(int n) {
  for (int i = 1; i < n; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
    delay(500);
  }
  digitalWrite(PIN_LED, HIGH);
  delay(1000);
  digitalWrite(PIN_LED, LOW);
  return n;
}

void flashEnd() {
  digitalWrite(PIN_LED, HIGH);
  delay(200);
  digitalWrite(PIN_LED, LOW);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButtonPressed, FALLING);

  IrReceiver.begin(PIN_IR_RECEIVE, ENABLE_LED_FEEDBACK);

  randomSeed(analogRead(A0));
}

void loop() {
  if (state == STATE_INIT) {
    delay(5000);

    nbBlinks1 = random(3, 7);   // 3..6
    blinkSeries300(nbBlinks1);
    delay(1000);

    nbBlinks2 = random(3, 7);   // 3..6
    blinkSeries300(nbBlinks2);
    delay(1000);

    nbBlinks3 = random(1, 4);   // 1..3
    blinkFinalSeries500(nbBlinks3);

    Serial.printf("nbBlinks1=%d nbBlinks2=%d nbBlinks3=%d\n", nbBlinks1, nbBlinks2, nbBlinks3);

    // clear any pending inputs collected during the blinking
    buttonEventFlag = false;
    piezoArmed = true;
    while (IrReceiver.decode()) IrReceiver.resume();

    if (nbBlinks3 == 1) {
      targetButton = nbBlinks1;
      targetPiezo  = nbBlinks2;
      doneButton = 0;
      donePiezo  = 0;
      expectingButton = true;      
      state = STATE_MODE_BUTTON_FIRST;
    } else if (nbBlinks3 == 2) {
      targetPiezo  = nbBlinks1;
      targetButton = nbBlinks2;
      doneButton = 0;
      donePiezo  = 0;
      expectingButton = false;     
      state = STATE_MODE_PIEZO_FIRST;
    } else { // nbBlinks3 == 3
      int total = nbBlinks1 + nbBlinks2;  // 6..12
      expectedLen = sprintf(expectedDigits, "%d", total); // e.g. "6" or "12"
      digitIndex = 0;
      Serial.printf("Type this number with the remote: %d\n", total);
      state = STATE_MODE_IR;
    }

    t1 = millis();

  } else if (state == STATE_MODE_BUTTON_FIRST || state == STATE_MODE_PIEZO_FIRST) {

    bool buttonClicked = checkButtonClickOnce();
    bool piezoHit       = checkPiezoHitOnce();

    bool buttonPending = doneButton < targetButton;
    bool piezoPending   = donePiezo < targetPiezo;

    if (buttonPending && piezoPending) {
      if (expectingButton && buttonClicked) {
        doneButton++;
        expectingButton = false;
      } else if (!expectingButton && piezoHit) {
        donePiezo++;
        expectingButton = true;
      }
    } else if (buttonPending) {
      if (buttonClicked) doneButton++;
    } else if (piezoPending) {
      if (piezoHit) donePiezo++;
    }

    if (doneButton >= targetButton && donePiezo >= targetPiezo) {
      t2 = millis();
      Serial.printf("t2=%lu  final time = %lu ms\n", t2, t2 - t1);
      flashEnd();
      state = STATE_INIT;
    }

  } else if (state == STATE_MODE_IR) {

    if (IrReceiver.decode()) {
      // Ignore key repeat frames
      if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
        
        // Match decoded raw 32-bit data or command
        uint32_t receivedCmd = IrReceiver.decodedIRData.decodedRawData;
        int digit = commandToDigit(receivedCmd);

        // Fallback check if your IR library version populates .command instead
        if (digit == -1) {
          digit = commandToDigit(IrReceiver.decodedIRData.command);
        }

        if (digit != -1) {
          int expectedDigit = expectedDigits[digitIndex] - '0';
          
          if (digit == expectedDigit) {
            digitIndex++;
            Serial.printf("Correct digit %d (%d/%d)\n", digit, digitIndex, expectedLen);

            if (digitIndex >= expectedLen) {
              t2 = millis();
              Serial.printf("t2=%lu  final time = %lu ms\n", t2, t2 - t1);
              flashEnd();
              state = STATE_INIT;
            }
          } else {
            Serial.printf("Wrong digit: received %d, expected %d\n", digit, expectedDigit);
          }
        }
      }
      IrReceiver.resume();
      delay(150); // Short refractory delay to prevent accidental double taps
    }
  }
}