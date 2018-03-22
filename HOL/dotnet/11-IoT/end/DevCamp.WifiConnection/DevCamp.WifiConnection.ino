#include <ESP8266WiFi.h>

// Pins
#define LED_PIN D4 // build-in LED in NodeMCU

// Wifi
const char ssid[] = "***";
const char password[] = "***";

void setup() {
  Serial.begin(115200); // sets up serial data transmission for status information
  
  pinMode(LED_PIN, OUTPUT); // configure the LED as an output pin
  
  connectWifi(ssid, password);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH); // turn the LED off
  } else {
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
  }
}

void connectWifi(const char ssid[], const char password[]) {
  Serial.print("Connecting to Wi-Fi");
  
  WiFi.hostname("NodeMCU@DevCamp");
  WiFi.begin(ssid, password);
  
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 50) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(".");
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Could not connect to Wi-Fi");
  } else {
    Serial.print("Connected to Wi-Fi: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}
