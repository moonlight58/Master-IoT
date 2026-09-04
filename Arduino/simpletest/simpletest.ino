#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(10);
 
  // power off Wifi to save energy
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(10); 
 
  pinMode(D2,INPUT); // switch
  pinMode(D1,OUTPUT); // LED
  digitalWrite(D1,LOW); // switch off the led at start
  delay(10);  
}
 
void testSwitch() {
  int state = digitalRead(D2);  
  if (state == 0) {
    Serial.println("switch off => light off");
    digitalWrite(D1,LOW);
  }
  else {
    Serial.println("switch on => light on");
    digitalWrite(D1,HIGH);    
  }  
}
 
void loop() {
  testSwitch();
  delay(10); // attendre 10ms
}