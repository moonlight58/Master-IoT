#include <ESP8266WiFi.h>

const char *ssid     = "testingiot";
const char *password = "testingiot";

const char    *serverIp   = "192.168.0.2";
const uint16_t serverPort = 12345;

const int PIN_BUTTON = D2;
const int PIN_LED    = D1; // Change to your LED pin if using an external LED

// Interrupt variables must be declared volatile
volatile bool mustSendRequest = false;
volatile int buttonState = 0;

WiFiClient client;
String clientId;

// Interrupt Service Routine (ISR)
// Note: ICACHE_RAM_ATTR is required for ESP8266 interrupts
void IRAM_ATTR handleButtonInterrupt() {
  buttonState = digitalRead(PIN_BUTTON);
  
  // Update LED state based on button position
  if (buttonState == HIGH) {
    digitalWrite(PIN_LED, HIGH); // Light on LED
  } else {
    digitalWrite(PIN_LED, LOW);  // Light off LED
  }
  
  mustSendRequest = true;
}

void setupWifi() {
  WiFi.setAutoConnect(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

String readLine() {
  String line = "";
  char c;

  while (true) {
    while (!client.available()) {
      delay(1);
    }
    c = client.read();
    if (c == '\n') {
      break;
    }
    if (c != '\r') {
      line += c;
    }
  }
  return line;
}

void connectToServer() {
  Serial.print("Connecting to server");
  while (!client.connect(serverIp, serverPort)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to server");

  // Read client ID immediately after connecting
  clientId = readLine();
  Serial.print("Client id: ");
  Serial.println(clientId);
}

void sendButtonRequest(int state) {
  // Format: BUTTON id_client button_state
  String request = "BUTTON " + clientId + " " + String(state);
  client.println(request);

  // Read response line
  String response = readLine();
  Serial.print("Server response: ");
  Serial.println(response);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);

  setupWifi();
  connectToServer();

  // Attach interrupt to pin D2 on any logic level change
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), handleButtonInterrupt, CHANGE);
}

void loop() {
  // Check if an interrupt flagged a button state change
  if (mustSendRequest) {
    // Temporarily store state and safely reset flag
    int currentState = buttonState;
    mustSendRequest = false;

    // Send request to server outside the ISR context
    sendButtonRequest(currentState);
  }

  // Small delay to prevent tight-loop CPU hogging
  delay(10);
}