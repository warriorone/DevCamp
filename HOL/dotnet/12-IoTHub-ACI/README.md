# Hands on Lab - IoT Hub

## Overview

In this lab you will setup an Azure IoT Hub, Create Devices, develop a .NET Core simulated device application, and deploy it as an Azure Container Instance.

## Objectives

In this hands-on lab, you will learn how to:

* Deploy an IoT HUB and create devices
* Develop the IoT Hub .NET Applications:
    * App to create the devices in the IoT Hub
    * App to simulate devices sending telemetry messages
    * App to send device to cloud messages
    * App to read cloud to device messages
* Build a docker image of the simulated device
* Deploy a simulated device as a hosted container in Azure Container Instances (ACI)

## Prerequisites

* You will need to have docker installed on your Windows developer workstation. Docker can be found here  [here](https://www.docker.com/docker-windows)
* Azure Storage Explorer, which can be found [here](https://azure.microsoft.com/en-us/features/storage-explorer/)
* The source for the starter app is located in the [start](start) folder
* The finished project is located in the [end](end) folder
* Deployed the starter ARM Template [HOL 12](../01-developer-environment).

## Exercises

This hands-on-lab has the following exercises:

* [Exercise 1: Create the IoT Hub](#ex1)
* [Exercise 2: Create the Apps](#ex2)
* [Exercise 3: Build the Docker image](#ex3)
* [Exercise 4: Deploy the image to Azure](#ex4)

    > ***In the hands-on-labs you will be using Visual Studio Solutions. Please > do not update the NuGet packages to the latest available, as we have not > tested the labs with every potential combination of packages.***

---
# Exercise 1: Create the IoT Hub<a name="ex1"></a>

## IoT Hub Creation

### Create an IoT hub for our simulated devices to send messages to. 

1.  Sign in to the Azure Portal and select New > Internet of Things > IoT Hub

    ![image](./media/12-net-ex1-a.PNG)
    
1. Choose a name for the IoT Hub, Pricing Tier, IoT Hub Units, Resource Group Name, and Location (e.g. West US) and click 'Create'

    ![image](./media/12-net-ex1-b.PNG)

1. Make note of the IoT Hub Hostname.

    ![image](./media/12-net-ex1-c.PNG)

1. Click on `Shared access policies > iothubowner` and note the **Primary Key** and **Connection String**.

    ![image](./media/12-net-ex1-d.PNG)

## Send IoT Cold Path messages to Storage

Now let's configure the IoT Hub to output messages to an Azure Storage Table Account.

1. In the Azure Portal Create an Azure Storage Account

    ![image](./media/12-net-ex1-2-a.PNG)

1. In the Azure Portal Create an Azure Function App, make sure to Select Existing for the Storage Account and select the storage account created in the previous step.

    ![image](./media/12-net-ex1-2-b.PNG)

1. Once the function is created click on the Azure Function in the Portal and add a new function.

    ![image](./media/12-net-ex1-2-c.PNG)

1. Click `custom function`

    ![image](./media/12-net-ex1-2-d.PNG)

1. Choose `JavaScript` in the language dropdown, Core in the Scenario dropdown, and select `EventHubTrigger - JavaScript`

    ![image](./media/12-net-ex1-2-e.PNG)

1. Next to the Event Hub connection box, select the `new` link

    ![image](./media/12-net-ex1-2-f.PNG)

1. In the dialog box that opens select IoT Hub, your IoT Hub, then Events as the Endpoint.

    ![image](./media/12-net-ex1-2-g.PNG)

1. Leave the other values with their defaults and click Create It should look something like this.

     ![image](./media/12-net-ex1-2-h.PNG)
    
1. This will open up the index.js file where you should add the following JavaScript

    ```JavaScript
    'use strict';
    module.exports = function (context, iotHubMessage) {
        context.log('Message received: ' + JSON.stringify(iotHubMessage));
        var date = Date.now();
        var partitionKey = Math.floor(date / (24 * 60 * 60 * 1000)) + '';
        var rowKey = date + '';
        context.bindings.outputTable = {
            "partitionKey": partitionKey,
            "rowKey": rowKey,
            "message": JSON.stringify(iotHubMessage)
        };
        context.done();
    };
    ```

1. Now click on the Integrate section of the Function > then click on +New Output > Choose `Azure Table Storage > Select`

    ![image](./media/12-net-ex1-2-i.PNG)

1. Leave the default values click Save

    ![image](./media/12-net-ex1-2-j.PNG)

1. Now use the Storage Explorer to connect to the Azure Table Storage so we can see the device messages stored in the Table. You will need to get the Storage Account name and keys from the portal.

    ![image](./media/12-net-ex1-2-k.PNG)

1. Open Storage Explorer > Connect to Azure Storage > Use Storage account name and key

    ![image](./media/12-net-ex1-2-l.PNG)

1. Enter your storage account name and key and click next > click Connect.

    ![image](./media/12-net-ex1-2-m.PNG)

## Exercise 2: Create the Apps<a name="ex2"></a>

Create the apps to create the devices in the IoT Hub and the app to simulate a device sending messages to the IoT Hub.

### Device Creation App

This application will create multiple devices in the IoT Hub

1. Open Visual Studio 2017 and create a new console application Named `CreateDevices`

    ![image](./media/12-net-ex2-a.PNG)
1. Right click on References in the Solution Explorer > Manage Nuget Packages > Browse > and search for `Microsoft.Azure.Devices` > Click `Install`

    ![image](./media/12-net-ex2-b.PNG)

1. Click on OK in the Review Changes dialog > Then I Accept on the License Acceptance dialog

    ![image](./media/12-net-ex2-c.PNG)

    ![image](./media/12-net-ex2-d.PNG)

1. Open Program.cs and add the following at the top of the file

    ```csharp
    using Microsoft.Azure.Devices;
    using Microsoft.Azure.Devices.Common.Exceptions;
    ```

1. Inside the Program Class add the following. Replace the `<IoT Hub Connection String>` placeholder with your IoT Hub Connection string:

    ```csharp
    static RegistryManager registryManager;
    static string connectionString = "<IoT Hub Connection String>";

    static string[] deviceName = new string[18] { "k1tx1", "k1tx2", "k1tx3", "k1tx4", "k1tx5", "k1tx6", "k1tx7", "k1tx8", "k1tx9", "sthlk1", "sthlk2", "sthlk3", "sthlk4", "sthlk5", "sthlk6", "sthlk7", "sthlk8", "sthlk9" };
    
    static string[] latitude   = new string[18] { "32.924276", "32.923664", "32.923556", "32.923556", "32.922763", "32.922079", "32.921502", "32.92107", "32.920566", "32.938589", "32.937923", "32.937455", "32.93668", "32.936338", "32.935366", "32.934321", "32.932736", "32.930341" };
    
    static string[] longitude  = new string[18] { "-97.295136", "-97.296081", "-97.29681", "-97.297626", "-97.297668", "-97.297497", "-97.296853", "-97.295866", "-97.294836", "-97.14298", "-97.143431", "-97.143753", "-97.144439", "-97.14474", "-97.144954", "-97.14489", "-97.144654", "-97.142723" };
    ```

1. Add the following function to the class. This will add the device into the IoT Hub and write out the device key to the console that we will use later on in the lab.

    ```csharp
        private static async Task AddDeviceAsync(string name, string lat, string lon)
        {
            string deviceId = name;
            Device device;
            try
            {
                device = await registryManager.AddDeviceAsync(new Device(deviceId));
            }
            catch (DeviceAlreadyExistsException)
            {
                device = await registryManager.GetDeviceAsync(deviceId);
            }
            Console.Write("'{0}', ", device.Authentication.SymmetricKey.PrimaryKey);
        }
    ```

1. Add the following code into the Main method.

    ```csharp
    registryManager = RegistryManager.CreateFromConnectionString(connectionString);
    for (int i = 0; i < deviceName.Length; i++)
    {
        AddDeviceAsync(deviceName[i], latitude[i], longitude[i]).Wait();
    }
    Console.ReadLine();
    ```

1. Now press `F5` or `Run` to debug the application. It will create all the devices in the IoT Hub and output their keys to the console. Make sure to copy the out put from the console. We will use that later in the lab.

    ![image](./media/12-net-ex2-e.PNG)

    If you forget to copy the console output, you can also get the device keys from the Azure Portal > Your IoT Hub > Device Explorer > Click on the device.

    ![image](./media/12-net-ex2-f.PNG)

## Device Simulator Application

1. Add a .NET Core Console project to the current solution in Visual Studio 2017 called `CoreSimulatedDevice`.

    ![image](./media/12-net-ex2-g.PNG)

1. Right click on Dependencies in the solution explorer and click on `Manage NuGet packages > Browse`. Enter `Microsoft.Azure.Devices.Client` then `Install`

    ![image](./media/12-net-ex2-h.PNG)

1. Click on `Ok` on the Review Changes dialog

    ![image](./media/12-net-ex2-i.PNG)

1. Click on `I Accept` in the License Acceptance dialog

    ![image](./media/12-net-ex2-j.PNG)

1. Add the following to the Program.cs file

    ```csharp
    using System.Text;
    using Microsoft.Azure.Devices.Client;
    using Newtonsoft.Json;
    using System.Threading.Tasks;

1. Add the following inside the CoreSimulated class

    ```csharp
    static DeviceClient deviceClient;
    static string iotHubUri = null;
    static string deviceKey = null;
    static string deviceName = null;
    static string deviceLatitude = null;
    static string deviceLongitude = null;
    ```

1. Add the following method to the CoreSimulated class

    ```csharp
     private static async Task SendDeviceToCloudMessagesAsync()
        {
            double minTemperature = 20;
            double minHumidity = 60;
            int messageId = 1;
            Random rand = new Random();

            while (true)
            {
                double currentTemperature = minTemperature + rand.NextDouble() * 15;
                double currentHumidity = minHumidity + rand.NextDouble() * 20;

                var telemetryDataPoint = new
                {
                    messageId = messageId++,
                    timestamp = System.DateTime.UtcNow.ToString(),
                    deviceId = deviceName,
                    temperature = currentTemperature,
                    humidity = currentHumidity,
                    longitude = deviceLatitude,
                    latitude = deviceLongitude
                };
                var messageString = JsonConvert.SerializeObject(telemetryDataPoint);
                var message = new Message(Encoding.ASCII.GetBytes(messageString));
                message.Properties.Add("temperatureAlert", (currentTemperature > 30) ? "true" : "false");

                Console.WriteLine("{0} > Sending message: {1}", DateTime.Now, messageString);

                await deviceClient.SendEventAsync(message);
                await Task.Delay(1000);
            }
        }
    ```

1. Add the following to the Main method

    ```csharp
    iotHubUri = Environment.GetEnvironmentVariable("IOTHUB_URI");
    deviceName = Environment.GetEnvironmentVariable("DEVICE_NAME");
    deviceKey = Environment.GetEnvironmentVariable("DEVICE_KEY");
    deviceLatitude = Environment.GetEnvironmentVariable("DEVICE_LATITUDE");
    deviceLongitude = Environment.GetEnvironmentVariable("DEVICE_LONGITUDE");

    Console.WriteLine("Device Name: " + deviceName);
    Console.WriteLine("Device Key: " + deviceKey);
    Console.WriteLine("Device Latitude: " + deviceLatitude);
    Console.WriteLine("Device Longitude: " + deviceLongitude);

    deviceClient = DeviceClient.Create(iotHubUri, new DeviceAuthenticationWithRegistrySymmetricKey(deviceName, deviceKey), TransportType.Mqtt);
    SendDeviceToCloudMessagesAsync().Wait();
    ```

1. We will need a device id, it's key, latitude, and longitude to test our console application. We can use the first device we have defined in the earlier application.

    * IOT Hub URI: Get that from the Azure Portal
    * Device Name: k1tx1
    * Device Key: Get that from the console output or from the Azure portal in the IoT Hub Device Explorer
    * Latitude: "32.924276"
    * Longitude: "-97.295136"

1. Now right click on the CoreSimulatedDevice Project in the `Solution Explorer > Properties > Debug > and add Environment Variables` and add the following Environment Variables and set the values to what was collected above.

    * IOTHUB_URI
    * DEVICE_NAME
    * DEVICE_KEY
    * DEVICE_LATITUDE
    * DEVICE_LONGITUDE

    ![image](./media/12-net-ex2-k.PNG)

1. Right click on your solution in the Solution Explorer > Properties > Startup Project and select Single startup project and select `CoreSimulatedDevice` from the dropdown.

    ![image](./media/12-net-ex2-l.PNG)

1. Now press F5 or Run to debug the application. We should see messages being sent to the IoT Hub.

    ![image](./media/12-net-ex2-m.PNG)

## Exercise 3: Build the Docker Image<a name="ex3"></a>

### Build the Docker Image

Now we will put our CoreSimulatedDevice into a docker image. First we will do it locally to test it then we will deploy

1. In Visual Studio 2017 in the Solution Explorer righ click on the CoreSimulatedDevice project > Add > Docker Support 

    ![image](./media/12-net-ex3-a.PNG)

1. Click on `Linux > Ok`

    ![image](./media/12-net-ex3-b.PNG)

1. This will add a Dockerfile to the CoreSimulatedDevice project and will also add a new project to the solution called "docker-compose"

    ![image](./media/12-net-ex3-c.PNG)

1. To build the container image, we need to create a **release** version of the CoreSimulated Device. To do this change the dropdown in the solution configuration dropdown to **Release**.

    ![image](./media/12-net-ex3-k.PNG)

1. Build the solution

    ![image](./media/12-net-ex3-l.PNG)

1. Open a command prompt or powershell terminal and change directory to the path of your visual studio solution and into the CoreSimulatedDevice project and run the following command to see what images you have running in your local Docker environment. If you just installed Docker for this lab, you should not see any images.

    ```powershell
    docker images
    ```

    ![image](./media/12-net-ex3-e.PNG)

1. Again, make sure you are in the path of your solution and the CoreSimualtedDevice Project( e.g. `D:\Code\Visual Studio 2017\Project\CreatedDevices\CoreSimulatedDevice`)

    ```powershell
    docker build -t coresimulateddevice .
    ```

![image](./media/12-net-ex3-f.PNG)

1. Now run the following command again, and you should see our coresimulateddevice and the microsoft/dotnet images in your local docker repository

    ```powershell
    docker images
    ```

![image](./media/12-net-ex3-g.PNG)

1. Now let's create a local container to test that our simulated device can run from a container. Replace the `<YOUR IOT HUB URI>` placeholder with the IoT Hub URI and the `<YOUR DEVICE KEY>` placeholder with the values from above

    ```powershell
    docker run -e "IOTHUB_URI=<YOUR IOT HUB URI>" -e "DEVICE_NAME=k1tx1" -e "DEVICE_KEY=<YOUR DEVICE KEY>" -e "DEVICE_LATITUDE=32.924276" -e "DEVICE_LONGITUDE=-97.295136" coresimulateddevice k1tx1
    ```

1. You should see the container sending messages to the IOT HUB

    ![image](./media/12-net-ex3-h.PNG)

1. You can also use the Azure Storage Explorer to validate that the message are making it to the IOT Hub and our Table Storage.

    ![image](./media/12-net-ex3-i.PNG)

## Exercise 4: Deploy the image to Azure<a name="ex4"></a>

### Deploy and Azure Image Registry and push your image

1. In the Azure Portal Choose + New > Containers > Azure Container Registry (ACR)

    ![image](./media/12-net-ex4-1-a.PNG)

1. Once the ACR is created make sure to enable the Admin User by click on the Access Keys > Admin user > Enable. Then make note of the:

    * Registry Name
    * Login Server
    * User Name:
    * Password:

    ![image](./media/12-net-ex4-1-b.PNG)

1. Now let's tag our local image for the ACR in a PowerShell Console and push it to the ACR

    ```powershell
    $resourceGroupName = "<your resource group name>"
    $acrName = "<your acr name>" #DO NOT INCLUDE THE .azurecr.io SUFFIX 

    Login-AzureRmAccount
    Select-AzureRmSubscription -SubscriptionId "<your subscription id>"

    $acr = Get-AzureRmContainerRegistry -ResourceGroupName $resourceGroupName -RegistryName $acrName
    $acrImageName = $acr.LoginServer + "/coresimulateddevice:v1"

    docker tag coresimulateddevice $acrImageName

    $acrCreds = Get-AzureRmContainerRegistryCredential -RegistryName $acrName -ResourceGroupName $resourceGroupName

    docker login $acr.LoginServer -u $acrCreds.Username -p $acrCreds.Password
    docker push $acrImageName
    ```

1. You should see the image pushed the the ACR

    ![image](./media/12-net-ex4-1-c.PNG)

1. And in the Azure Portal

    ![image](./media/12-net-ex4-1-d.PNG)

## Deploy an Azure Container Instance (ACI) for each registered device to simulate load

1. In this lab we'll use PowerShell to deploy an ACI for each device we want to simulate

1. Create a PowerShell Script to deploy the instances

    ```powershell
    Login-AzureRmAccount
    Select-AzureRmSubscription -Subscription "<your subscription here>"

    $rgName = "<YOUR RESOURCE GROUP NAME>"
    $acrName = "<YOUR ACR NAME>"
    $iotHubUri = "<YOUR IOT HUB URI>"
    $location = "<YOUR RG LOCATION>"
    $imageName = "<your acr login server>/coresimulateddevice:v1"

    $deviceName = @("k1tx1", "k1tx2", "k1tx3", "k1tx4", "k1tx5", "k1tx6", "k1tx7", "k1tx8", "k1tx9", "sthlk1", "sthlk2", "sthlk3", "sthlk4", "sthlk5", "sthlk6", "sthlk7", "sthlk8", "sthlk9" )

    $latitude   = @( "32.924276", "32.923664", "32.923556", "32.923556", "32.922763", "32.922079", "32.921502", "32.92107", "32.920566", "32.938589", "32.937923", "32.937455", "32.93668", "32.936338", "32.935366", "32.934321", "32.932736", "32.930341" )

    $longitude  = @( "-97.295136", "-97.296081", "-97.29681", "-97.297626", "-97.297668", "-97.297497", "-97.296853", "-97.295866", "-97.294836", "-97.14298", "-97.143431", "-97.143753", "-97.144439", "-97.14474", "-97.144954", "-97.14489", "-97.144654", "-97.142723" )

    $keys = @('<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>', '<device keys here>')

    $acr = Get-AzureRmContainerRegistry -ResourceGroupName $rgName -Name $acrName
    $loginServer = $acr.LoginServer
    $acrCreds = Get-AzureRmContainerRegistryCredential -RegistryName $acrName -ResourceGroupName $rgName 
    $secpasswd = ConvertTo-SecureString $acrCreds.Password -AsPlainText -Force
    $acrcred = New-Object System.Management.Automation.PSCredential ($acrCreds.Username, $secpasswd)

    ##Create a single container to test
    #Write-Host "deviceName: " $deviceName[0] 
    #Write-Host "deviceLat: "  $latitude[0] 
    #Write-Host "deviceLong: " $longitude[0] 
    #Write-Host "key: " $keys[0]
    #$EnvironmentHash = @{"IOTHUB_URI"=$loginServer;"DEVICE_NAME"=$deviceName[0];"DEVICE_KEY"=$keys[0];"DEVICE_LATITUDE"=$latitude[0];"DEVICE_LONGITUDE"=$longitude[0]}
    #New-AzureRmContainerGroup -ResourceGroupName $rgName -Name $deviceName[$i] -Image $imageName `
    #    -Location $location -OsType Linux -Cpu 1 -MemoryInGB 1 -IpAddressType Public -RegistryCredential $acrcred -EnvironmentVariable $EnvironmentHash
    ##Show the Console output from the first container
    #Get-AzureRmContainerInstanceLog -ContainerGroupName $deviceName[0] -ResourceGroupName $rgName
    ##Remove the test ACI
    #Remove-AzureRmContainerGroup -ResourceGroupName $rgName -Name $deviceName[0]

    #Create all the containers
    for($i=0; $i -lt $deviceName.Length; $i++)
    {
        Write-Host "deviceName: " $deviceName[$i] 
        Write-Host "deviceLat: "  $latitude[$i] 
        Write-Host "deviceLong: " $longitude[$i] 
        Write-Host "key: " $keys[$i]
        $EnvironmentHash = @{"IOTHUB_URI"=$loginServer;"DEVICE_NAME"=$deviceName[$i];"DEVICE_KEY"=$keys[$i];"DEVICE_LATITUDE"=$latitude[$i];"DEVICE_LONGITUDE"=$longitude[$i]}
        New-AzureRmContainerGroup -ResourceGroupName $rgName -Name $deviceName[$i] -Image $imageName `
        -Location $location -OsType Linux -Cpu 1 -MemoryInGB 1 -IpAddressType Public -RegistryCredential $acrcred -EnvironmentVariable $EnvironmentHash
    }


    ##Clean up Containers when done testing
    for($i=0; $i -lt $deviceName.Length; $i++)
    {
        Remove-AzureRmContainerGroup -ResourceGroupName $rgName -Name $deviceName[$i]
    }
    ```

1. If you deploy all the device containers you should see something similar to this in the output terminal

    ![image](./media/12-net-ex4-1-e.PNG)

1. You should also see them created in the portal

    ![image](./media/12-net-ex4-1-f.PNG)

1. You should also see messages in the Azure Table Storage from multiple devices now

    ![image](./media/12-net-ex4-1-g.PNG)

----

Copyright 2018 Microsoft Corporation. All rights reserved. Except where otherwise noted, these materials are licensed under the terms of the MIT License. You may use them according to the license as is most appropriate for your project. The terms of this license can be found at https://opensource.org/licenses/MIT.