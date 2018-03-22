#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

// Pins
#define LED_PIN D4 // build-in LED in NodeMCU

// Wifi
const char ssid[] = "***";
const char password[] = "***";

WiFiClient client;

// Request
const int port = 80;
const char serverUrl[] = "incidentapi[YOUR_RG_NAME].azurewebsites.net"; // address for request, without http://

const char requestIncidentsUri[] = "/incidents";
const char requestIncidentUri[] = "/incidents/"; // usage: /incidents/{IncidentId}
const char requestIncidentsCountUri[] = "/incidents/count";
const char requestIncidentsCountIncludeResolved[] = "/incidents/count/includeresolved";

// Telegram bot
#define BOT_TOKEN "123456789:AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqR" // bot token to access the HTTP API

WiFiClientSecure clientSecure;
UniversalTelegramBot bot(BOT_TOKEN, clientSecure);

const int maxMessageLength = 1300;  // maximum length of a message defined in UniversalTelegramBot.h
const int interval = 1000; // mean time between scan messages
long lastMillis; // last time messages' scan has been done

void setup() {
  Serial.begin(115200); // sets up serial data transmission for status information

  pinMode(LED_PIN, OUTPUT);

  connectWifi(ssid, password);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    if (millis() > lastMillis + interval) { // better approach to pause between each updates
      int messageCount = bot.getUpdates(bot.last_message_received + 1);
      if (messageCount < 35) {
        while (messageCount) {
          handleMessages(messageCount);
          messageCount = bot.getUpdates(bot.last_message_received + 1);
        }
      }
      lastMillis = millis();
    }
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

String getIncidents() {
  String incidents;

  WiFiClient client;
  if (client.connect(serverUrl, port)) {
    Serial.print("Connected to ");
    Serial.println(serverUrl);
    Serial.println("Sending request");

    client.print("GET ");
    client.print(requestIncidentsUri);
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
      if (client.available()) {
        while (client.findUntil("{", "\0")) {
          String result = client.readStringUntil('}');

          String id = result.substring(result.indexOf("\"id\":\"") + 6, result.indexOf("\",\"Description\""));
          // removing the hyphens from the id so the command formatting in the Telegram client doesn't break
          id.replace("-", "");

          String description = result.substring(result.indexOf("\"Description\":\"") + 15, result.indexOf("\",\"Street\""));
          if (description == "") {
            continue; // ignore incidents without description
          }
          description.replace("<br />", "\n");

          String incident =
            description + "\n"
            "/incident_" + id + "\n\n";

          if (incident.length() + incidents.length() > maxMessageLength) { // truncate message to avoid a memory leak
            return incidents;
          } else {
            incidents += incident;
          }
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
  return incidents;
}

void handleMessages(int messageCount) {
  Serial.println("handleMessages(" + String(messageCount) + ")");
  for (int i = 0; i < messageCount; i++) {
    if (bot.messages[i].chat_id != "") { // chat id may never be empty
      Serial.println("Chat ID: " + bot.messages[i].chat_id);
      Serial.println("From ID: " + bot.messages[i].from_id);
      Serial.println("From name: " + bot.messages[i].from_name);
      Serial.println("Text: " + bot.messages[i].text);

      String fromName = bot.messages[i].from_name;
      if (fromName == "") {
        fromName = "Guest";
      }

      if (bot.messages[i].text == "/start") {
        bot.sendMessage(bot.messages[i].from_id,
          "Welcome, " + fromName + "!\n\n"
          "You can control me by sending these commands:\n\n"
          "<strong>Manage Incidents</strong>\n"
          "/incidents - show all incidents\n"
          "/incident_{IncidentId} - show specific incident\n"
          "/incidents_count - get amount of incidents\n"
          "/incidents_count_includeresolved - get amount of incidents including resolved incidents",
          "HTML");
      } else if (bot.messages[i].text == "/incidents") {
        bot.sendChatAction(bot.messages[i].chat_id, "typing");

        String incidents = getIncidents();

        // send a message with the current incident to bot subscriber
        bot.sendMessage(bot.messages[i].from_id, incidents);
      } else if (bot.messages[i].text.startsWith("/incident_")) {
        bot.sendChatAction(bot.messages[i].chat_id, "typing");

        String incidentId = bot.messages[i].text.substring(10);
        if (incidentId == "") {
          bot.sendMessage(bot.messages[i].from_id, "There was no ID specified. Please use /incidents and select one of the listed commands.");
        } else {
          // adding hyphens to the id to restore the GUID format
          incidentId =
            incidentId.substring(0, 8) + "-" +
            incidentId.substring(8, 12) + "-" +
            incidentId.substring(12, 16) + "-" +
            incidentId.substring(16, 20) + "-" +
            incidentId.substring(20);

          String result = sendRequest(requestIncidentUri + incidentId);

          // parsing the JSON string
          String id = result.substring(result.indexOf("\"id\":\"") + 6, result.indexOf("\",\"Description\""));
          if (id != "" && id == incidentId) { // if an incident with the specified ID exists the IDs will match
            String description = result.substring(result.indexOf("\"Description\":\"") + 15, result.indexOf("\",\"Street\""));
            String street = result.substring(result.indexOf("\"Street\":\"") + 10, result.indexOf("\",\"City\""));
            String city = result.substring(result.indexOf("\"City\":\"") + 8, result.indexOf("\",\"State\""));
            String state = result.substring(result.indexOf("\"State\":\"") + 9, result.indexOf("\",\"ZipCode\""));
            String zipCode = result.substring(result.indexOf("\"ZipCode\":\"") + 11, result.indexOf("\",\"FirstName\""));
            String firstName = result.substring(result.indexOf("\"FirstName\":\"") + 13, result.indexOf("\",\"LastName\""));
            String lastName = result.substring(result.indexOf("\"LastName\":\"") + 12, result.indexOf("\",\"PhoneNumber\""));
            String phoneNumber = result.substring(result.indexOf("\"PhoneNumber\":\"") + 15, result.indexOf("\",\"OutageType\""));

          bot.sendMessage(bot.messages[i].from_id,
            "<strong>" + description + "</strong>\n"
            "Street: " + street + "\n"
            "City: " + city + "\n"
            "State: " + state + "\n"
            "ZIP code: " + zipCode + "\n"
            "Name: " + firstName + " " + lastName + "\n"
            "Phone: " + phoneNumber,
            "HTML");
          } else {
            bot.sendMessage(bot.messages[i].from_id, "There is no incident with the specified ID.");
          }
        }
      } else if (bot.messages[i].text == "/incidents_count") {
        bot.sendChatAction(bot.messages[i].chat_id, "typing");

        // retrieve the amount of incidents
        String result = sendRequest(requestIncidentsCountUri);
        int incidentsCount = result.toInt();

        // send a message with the amount of incidents to all bot subscribers
        bot.sendMessage(bot.messages[i].from_id,
          "There are currently <strong>" + String(incidentsCount) + " incidents</strong> available.", "HTML");

        // keep the led blinking for the amount of incidents
        for (int i = 0; i < incidentsCount; i++) {
          delay(250);
          digitalWrite(LED_PIN, LOW);
          delay(50);
          digitalWrite(LED_PIN, HIGH);
        }
      } else if (bot.messages[i].text == "/incidents_count_includeresolved") {
        bot.sendChatAction(bot.messages[i].chat_id, "typing");

        String result = sendRequest(requestIncidentsCountIncludeResolved);
        int incidentsCountIncludeResolved = result.toInt();

        // send a message with the amount of incidents
        bot.sendMessage(bot.messages[i].from_id,
          "There are currently <strong>" + String(incidentsCountIncludeResolved) + " incidents including resolved incidents</strong> available.", "HTML");

        // keep the led blinking for the amount of incidents
        for (int i = 0; i < incidentsCountIncludeResolved; i++) {
          delay(250);
          digitalWrite(LED_PIN, LOW);
          delay(50);
          digitalWrite(LED_PIN, HIGH);
        }
      }
    }
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
        // looking for search string in response data
        if (client.findUntil("\r\n\r\n", "\0")) {
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

