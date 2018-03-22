#include <ESP8266WiFi.h>

// Pins
#define LED_PIN D4 // build-in LED in NodeMCU

// Wifi
const char ssid[] = "***";
const char password[] = "***";

WiFiClient client;

// Request
const int port = 80;
const char serverUrl[] = "incidentapi[YOUR_RG_NAME].azurewebsites.net"; // address for request, without http://

const char requestIncidentsCountUri[] = "/incidents/count";

void setup() {
  Serial.begin(115200); // sets up serial data transmission for status information
  
  pinMode(LED_PIN, OUTPUT);
  
  connectWifi(ssid, password);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
	  digitalWrite(LED_PIN, HIGH);
  } else {
  	// retrieve the amount of incidents
  	String result = sendRequest(requestIncidentsCountUri);
    if (result != "") {
      Serial.print("The incident count is: ");
      Serial.println(result);
      
		  int incidentsCount = result.toInt();
		
  		// keep the led blinking for the amount of incidents
  		for (int i = 0; i < incidentsCount; i++) {
  		  delay(250);
  		  digitalWrite(LED_PIN, LOW);
  		  delay(50);
  		  digitalWrite(LED_PIN, HIGH);
  		}
  	} else {
      Serial.println("Result is empty");
  	}
  	// pause between requests
  	delay(60000);
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

String sendRequest(String uri) {
  if (client.connect(serverUrl, port)) {
    Serial.print("Connected to ");
    Serial.println(serverUrl);
    Serial.println("Sending request");
    
    client.print("GET ");
    client.print(uri);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.print(serverUrl);
    client.print(":");
    client.println(port);
    client.println("Connection: close");
    client.println("Accept: text/html");
    client.println();

    // waiting for server response...
    while (client.connected()) {
      // ...until the response is available
      while (client.available()) {
		    // looking for the first empty line that indicates the end of the header
        if (client.findUntil("\r\n\r\n", "\0")) {
		      // return the payload
          return client.readStringUntil('\n');
        }
      }
    }
    client.stop();

    Serial.println();
    Serial.println("Connection closed");
  } else {
    Serial.print("Connection to ");
    Serial.print(serverUrl);
    Serial.println(" failed");
  }
  return "";
}
