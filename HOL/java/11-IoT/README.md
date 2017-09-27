# IoT (Java)

## Overview
City Power & Light is a sample application that allows citizens to report "incidents" that have occurred in their community. It includes a landing screen, a dashboard, and a form for reporting new incidents with an optional photo. The application is implemented with several components:

* Front end web application contains the user interface and business logic. This component has been implemented three times in .NET, NodeJS, and Java.
* WebAPI is shared across the front ends and exposes the backend DocumentDB.
* DocumentDB is used as the data persistence layer.

In this lab, you will combine the web app with an IoT device based on an Arduino board that will query the app for the number of incidents and display the refreshed number every minute.

## Objectives
In this hands-on lab, you will learn how to:
* Set up the developing environment to support the programming of Arduino chips.
* Create your own IoT software from scratch.

## Prerequisites
* The source for the starter app is located in the [start](start) folder. 
* The finished project is located in the [end](end) folder. 
* Deployed the starter ARM Template [HOL 1](../01-developer-environment).
* Completion of the [HOL 5](../05-arm-cd).

## Exercises
This hands-on-lab has the following exercises:
* [Exercise 1: Set up your environment](#ex1)
* [Exercise 2: Create output that will be consumed by the device](#ex2)
* [Exercise 3: Program the device](#ex3)

---
## Exercise 1: Set up your environment<a name="ex1"></a>

To program an Arduino device on your machine you need ..., Visual Studio and ...

1. The easiest way to install ...

You have now installed all the necessary components to start programming an Arduino device on your machine.

---

## Exercise 2: Create output that will be consumed by the device<a name="ex2"></a>

The device will regularly call an URL to fetch the current incident count. We will add a page to our existing web application as an easy way to provide this data.

1. In the `DevcampApplication.java` file add `/IoT**` to the list of `antMatchers`.

1. Create a new Controller. Right-click on `devCamp.WebApp.Controllers` and select `New` -> `Class`.

    ![image](./media/2017-09-11_15_06_00.png)

1. In the `New Java Class` dialog enter `IoTController` as the name and click `Finish`.

    ![image](./media/2017-09-11_15_08_00.png)

1. The controller will just emulate the behavior of the dashboard controller. Add the following code to the newly created file:

    ```csharp
    package devCamp.WebApp.Controllers;
    
    import java.util.List;
    import java.util.concurrent.CompletableFuture;
    
    import devCamp.WebApp.models.IncidentBean;
    import devCamp.WebApp.services.IncidentService;
    import org.slf4j.Logger;
    import org.slf4j.LoggerFactory;
    import org.springframework.beans.factory.annotation.Autowired;
    import org.springframework.scheduling.annotation.Async;
    import org.springframework.stereotype.Controller;
    import org.springframework.ui.Model;
        import org.springframework.web.bind.annotation.RequestMapping;
        
    @Controller
    public class IoTController {
      private static final Logger LOG = LoggerFactory.getLogger(DashboardController.class);
    
      @Autowired
      IncidentService service;
    
      @RequestMapping("/IoT")
      public String iot(Model model) {
        List<IncidentBean> list = service.getAllIncidents();
        model.addAttribute("allIncidents", list);
        return "IoT/index";
      }  
    }


1. Create a new template. Right-click on `src/main/resources` and select `New` -> `Other...`. ...

    ![image](./media/2017-09-11_15_34_00.png)

1. In the `New` dialog select the `HTML File` template and click `Next >`.

    ![image](./media/2017-09-11_15_34_30.png)

1. Set the folder to `[...]/resources/templates/IoT` and the file name to `index.html`, then click `Finish`.

    ![image](./media/2017-09-11_15_34_40.png)

1. The template will only display the number of incidents. Add the following code to the newly created file:

    ```html
    IncidentCount=<span th:text="${allIncidents.size()}" th:remove="tag"></span>

1. Start the debugger and add `/IoT` to the URL to test the new view. It will contain just the number of incidents.

    ![image](./media/2017-09-12_11_07_00.png)

1. Publish the changes to your Azure web app and make sure the `/IoT` URL is reachable (see [HOL 5](../05-arm-cd)).
 
    ![image](./media/2017-09-26_15_54_00.png)

You have now created the data feed for your device.

---
## Exercise 3: Program the device<a name="ex3"></a>

It is important to develop projects in small chunks and to understand each function. Try to develop code with small functions that clearly separate the functionalities of your device and combine them step by step.

1. Open Arduino and create a new sketch.

1. Replace the sketch content with the following code which will connect the device to a specified wireless network. Replace the SSID and the password with proper values.

    ```cpp
    #include <ESP8266WiFi.h>

    // Pins
    #define LED_PIN   D4 // build-in LED in NodeMCU

    // Wifi
    char* ssid = "***";
    char* password = "***";

    void setup() {
      Serial.begin(115200); // sets up serial data transmission for status information
      
      pinMode(LED_PIN, OUTPUT);
      
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

    void connectWifi(char* ssid, char* password) {
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

1. When it comes to debugging code in Arduino, `Serial.print()` and `Serial.println()` are the best way to go. `Serial.begin(115200);` sets up serial data transmission with 115200 baud. Select `Tools` -> `Serial Monitor` to open the `Serial Monitor` dialog for the selected COM port.

    ![image](./media/arduino-tools-serial%20monitor.png)

1. In the `Serial Monitor` dialog select `115200 baud` from the list and leave the dialog open to see the output from the device.

    ![image](./media/arduino-serial%20monitor.png)

1. Let's test the wireless network connection. Hit `CTRL+U` to compile and upload the sketch to your device. If the connection to your network was established, the LED on your device will start blinking. It will completely turn off if the connection has failed.

    ![image](./media/arduino-connect%20to%20wifi-upload%20completed.png)

1. Now we will add an HTTP request to our Arduino code. The new code will also add the method `getIncidentCount()` which will fetch the current incident count from the `IoT` view created in `Exercise 2`. Use the code below to replace or adapt the existing code.

    ```cpp
    #include <ESP8266WiFi.h>

    // Pins
    #define LED_PIN   D4 // build-in LED in NodeMCU

    // Wifi
    char* ssid = "***";
    char* password = "***";

    // Request
    const int port = 80;
    const char* server = "<app_name>.azurewebsites.net/IoT"; // address for request, without http://
    const char* searchString = "IncidentCount="; // search for this property

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
        int incidentCount = getIncidentCount();
        
        // keep the led blinking for the amount of incidents
        for (int i = 0; i < incidentCount; i++) {
          delay(250);
          digitalWrite(LED_PIN, LOW);
          delay(50);
          digitalWrite(LED_PIN, HIGH);
        }
        // pause between requests
        delay(1000);
      }
    }

    void connectWifi(char* ssid, char* password) {
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

    int getIncidentCount() {
      WiFiClient client;
      if (client.connect(server, port)) {
        Serial.print("Connected to ");
        Serial.println(server);
        Serial.println("Sending request");
        
        client.print("GET /");
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.print(server);
        client.print(":");
        client.println(port);
        client.println("Connection: close");
        client.println("Accept: text/html");
        client.println();

        // waiting for server response until client.available() returns true
        while (client.connected()) {
          // looking for search string in response data
          if (client.available()) {
            if (client.findUntil(searchString, "\0")) {
              int result = client.readStringUntil('\n').toInt();
              
              Serial.print(searchString);
              Serial.print("=");
              Serial.println(result);
              
              return result;
            }
          }
        }
        client.stop();

        Serial.println();
        Serial.println("Connection closed");
      } else {
        Serial.print("Connection to ");
        Serial.print(server);
        Serial.println(" failed");
      }
      return 0;
    }

1. Replace the SSID and the password with proper values. Also replace the address in the following line with the address to the `IoT` view:

    ```cpp
    const char* server = "<app_name>.azurewebsites.net/IoT"; // address for request, without http://

1. Select `Sketch` -> `Upload` or use `CTRL+U` to compile and upload the sketch to your device. The device will retrieve the amount of incidents from the `IoT` view and flash the LED for this amount.

    ![image](./media/arduino-get%20incident%20count-upload%20completed.png)

This example shows how to work with data requests, how to link the device to a data source on the internet and display the state using a simple LED.

---
## Summary

In this hands-on lab, you learned how to:
* Set up the developing environment to support the programming of Arduino chips.
* Create your own IoT software from scratch.

---
Copyright 2017 Microsoft Corporation. All rights reserved. Except where otherwise noted, these materials are licensed under the terms of the MIT License. You may use them according to the license as is most appropriate for your project. The terms of this license can be found at https://opensource.org/licenses/MIT.