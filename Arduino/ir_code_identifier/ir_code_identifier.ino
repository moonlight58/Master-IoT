// STEP 1 : run this sketch first, open Serial Monitor at 115200 baud,
// and press each button 2..9 on your remote one by one.
// Note down the hex code printed for each digit - you'll need it
// in the final sketch (simplereflexes-ir.ino).

#include <IRremote.hpp>

#define PIN_IR_RECEIVE D3   // change to whatever pin your IR receiver's OUT is wired to

void setup() {
  Serial.begin(115200);
  delay(200);
  IrReceiver.begin(PIN_IR_RECEIVE, ENABLE_LED_FEEDBACK);
  Serial.println("\nReady. Press buttons on the remote...");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("Code (hex): 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume();  // ready to receive the next value
  }
}
