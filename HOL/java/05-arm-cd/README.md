# ARM (Java)

## Overview
In this lab, you will learn to provision and manage resources in Azure with the new Azure Resource Manager.  Then we will deploy our sample application into newly created infrastructure.

## Objectives
In this hands-on lab, you will learn how to:
* Author Azure Resource Manager templates.
* Deploy ARM Templates to Azure.
* Integrate environments into VSTS Release pipelines.
* Deploy City Power & Light to new Web App.

## Prerequisites

* You should have completed the previous Continuous Integration HOL.

## Exercises
This hands-on-lab has the following exercises:
* [Exercise 1: Create an ARM Template in Visual Studio Code](#ex1)
* [Exercise 2: Deploy ARM Template to Azure via the XPlat CLI](#ex2)
* [Exercise 3: Integrate new Web App into VSTS](#ex3)
* [Exercise 4: Deploy City Power & Light to new Web App](#ex4)

---
## Exercise 1: Create an ARM Template in Visual Studio Code<a name="ex1"></a>

You can use any editor you like to create Azure resource group templates, but both Visual Studio and Visual Studio Code have convenient mechanisms for this process. In this example, we are going to use Visual Studio Code, and we are going to leverage the [Azure Resource Manager Tools extension](https://marketplace.visualstudio.com/items?itemName=msazurermtools.azurerm-vscode-tools).

1. Install the ARM Tools extension in Visual Studio Code by using the [Command Palette](https://code.visualstudio.com/Docs/editor/codebasics#_command-palette). With VSCode open, press `CTRL` + `P` and enter `ext install azurerm-vscode-tools`.

    ![image](./media/image-001.gif)

    ![image](./media/2017-11-03_11_17_00.png)

    This extension gives intellisense and schema support for ARM Templates.
    
1. Next, let's install a pack of code snippets to make creating resources easier. In VS Code, you can open the JSON snippets file by either navigating to `File` -> `Preferences` -> `User Snippets` -> `JSON`, or by selecting `F1` and typing `preferences` until you can select `Preferences: Snippets`.

    ![image](./media/image-002.gif)

    From the options, select `JSON`:

    ![image](./media/image-003.gif)

    The json file that opens can be extended to hold custom snippets.  
    
    ![image](./media/image-004.gif)
    
    From the [azure-xplat-arm-tooling](https://github.com/Azure/azure-xplat-arm-tooling/) repo, open the [raw snippets file](https://raw.githubusercontent.com/Azure/azure-xplat-arm-tooling/master/VSCode/armsnippets.json) and copy the entire contents to your clipboard.

    Then, paste the contents into VSCode in between the `{}` of the json file, replacing the existing comment. 

    ![image](./media/image-005.gif)

    Save and close the file. You will now be able to use snippets in the creation of ARM files.

1. Now that we have our tooling setup, open `start` -> `azuredeploy.json` in this HOL's folder. This is a skeleton ARM Template, including the four sections `parameters`, `variables`, `resources`, and `outputs`. Click into the brackets next to `resources` and create a linebreak. In the new line, type `arm-p` and hit enter to select `arm-plan`. This will create a new App Service Plan, which controls the features and performance of associated Azure Web Apps.

    ![image](./media/image-006.gif)

    Tap the right arrow key, then hit backspace to remove the `1` from the resource name `AppServicePlan1`. 

    ![image](./media/image-007.gif)

    Next to our resource's ending `}` add a `,` and a line break. Then repeat the process above to create a Web App by typing `arm-w` and selecting `arm-webapp`.

    ![image](./media/image-008.gif) 

    This web app name needs to be globally unique, as it will be used for the https://[...].azurewebsites.net DNS entry and cannot be the same as an existing webapp. Use `javaapptest` plus 4-5 random characters.

    The webapp resource has stubbed in 3 instances of `APP_SERVICE_PLAN_NAME`. Replace this value with the `AppServicePlan` name value that you gave the App Service Plan earlier when you were removing the `1` from the resource name.

    ![image](./media/image-009.gif)

1. The web application needs to be configured to work with the AzureAD, Azure Storage, Azure Redis Cache, and ASP.NET WebAPI that we configured earlier.

    In earlier exercises we have configured these settings as environment variables on our local machines, and in the Azure Portal for our "Dev" Azure Web App.

    ARM Templates can include `resources`, which define numerous options for a given resource. For a web app, we can use `appsettings` to adjust the environment variables present on our app. Here is an extended web app with the `resources` array filled in.
    
    Paste the `resources` content into the ARM Web App resource within your ARM Template (as a nested resource). Update the `dependsOn` attribute to match your website's `name`, and the environment variables to match your values. 

    > If you are using VSCode and have been debugging locally with `.vscode/launch.json` then you can copy/paste the values into the template to override the sample values below:

    ```json
	{
		"apiVersion": "2015-08-01",
		"name": "[YOUR WEB APP NAME]",
		"type": "Microsoft.Web/sites",
		"location": "[resourceGroup().location]",
		"tags": {
			"[concat('hidden-related:', resourceGroup().id, '/providers/Microsoft.Web/serverfarms/AppServicePlan')]": "Resource",
			"displayName": "[YOUR WEB APP NAME]"
		},
		"dependsOn": [
			"Microsoft.Web/serverfarms/AppServicePlan"
		],
		"properties": {
			"name": "[YOUR WEB APP NAME]",
			"serverFarmId": "[resourceId('Microsoft.Web/serverfarms/', 'AppServicePlan')]"
		},
		"resources": [
		    {
				"apiVersion": "2015-08-01",
				"name": "web",
				"type": "config",
				"dependsOn": [
					"[concat('Microsoft.Web/sites/', '[YOUR WEB APP NAME]')]"
				],
				"properties": {
					"javaVersion": "1.8",
					"javaContainer": "TOMCAT",
					"javaContainerVersion": "8.0"
				}
			},
			{
				"name": "appsettings",
				"type": "config",
				"apiVersion": "2015-08-01",
				"dependsOn": [
					"[concat('Microsoft.Web/sites/', '[YOUR WEB APP NAME]')]"
				],

				"tags": {
					"displayName": "AppSettings"
				},
				"properties": {
					"WEBSITE_NODE_DEFAULT_VERSION": "6.7.0",
					"AZURE_STORAGE_ACCOUNT": "[YOUR STORAGE ACCOUNT]",
					"AZURE_STORAGE_ACCESS_KEY": "[YOUR STORAGE ACCOUNT KEY]",
					"AZURE_STORAGE_BLOB_CONTAINER": "images",
					"AZURE_STORAGE_QUEUE": "thumbnails",
					"INCIDENT_API_URL": "http://[YOUR INCIDENT WEB APP URL].azurewebsites.net/",
					"REDISCACHE_HOSTNAME": "[YOUR REDIS CACHE URL].redis.cache.windows.net",
					"REDISCACHE_PORT": "6379",
					"REDISCACHE_SSLPORT": "6380",
					"REDISCACHE_PRIMARY_KEY": "[YOUR REDIS CACHE KEY]",
					"AAD_CLIENT_ID": "[YOUR AAD CLIENT ID]",
					"AAD_CLIENT_SECRET": "[YOUR AAD CLIENT SECRET]",
					"AAD_RETURN_URL": "[concat('https://', reference('[YOUR WEB APP NAME]', '2015-08-01').defaultHostName, '/auth/openid/return')]"
				}
			}
		]
	}
    ```

    > For the `AAD_RETURN_URL` we are dynamically resolving the value by using a `reference()` lookup for a given app name. Ensure that you replace `[YOUR WEB APP NAME]` with whatever name you choose for your web app.

    Your template should now look like this:
   
    ![image](./media/2017-06-27_16_16_00.png)

We are now ready to deploy our ARM Template containing an App Service Plan, and a Web App with environment variables to Azure.

---
## Exercise 2: Deploy ARM Template to Azure via the XPlat CLI<a name="ex2"></a>

For deploying the ARM Template we will use the Azure Xplat CLI. Please ensure you have [installed](https://azure.microsoft.com/en-us/documentation/articles/xplat-cli-install/#option-1-install-an-npm-package) the Azure XPlat CLI package from NPM before proceeding.

1. From the command line, `cd` to the `start` directory containing our ARM Template:

    ```bash
    azure login
    ```

    Follow the on-screen instructions to log in via your browser.

    ```bash
    azure group create -n DevCampTest -l "West US"
    azure group deployment create -f .\azuredeploy.json -g DevCampTest
    ```
    
    > Feel free to swap out "West US" with another region.

    ![image](./media/2017-06-27_16_35_00.png)

1. Open the [Azure Portal](https://portal.azure.com) and verify that the app has been created in your resource group with the defined resources.

    ![image](./media/2017-06-27_16_39_00.png)

    Also check the `Application Settings` blade to verify that the environment variables were created as expected:

    ![image](./media/2017-06-27_16_41_00.png)

1. To use authentication with this new web app, we need to update our AzureAD app registration to whitelist its URL. In the browser, navigate to the [Application Registration Portal](https://apps.dev.microsoft.com/#/appList) and select your application. Under the `Platforms` heading, select `Add Url` and paste in the URL of your newly created Azure Web App plus the `/auth/openid/return` suffix. Also, since two of our applications share the same `*.azurewebsites.net` domain we need to add an entry for `https://azurewebsites.net` into the list. Click `Save`.

    ![image](./media/2017-06-27_16_44_00.png)
    
    > See [here](https://azure.microsoft.com/en-us/documentation/articles/active-directory-v2-limitations/#restrictions-on-redirect-uris) for more information about redirect URIs.

The resource group is now holding our "Test" environment web app and has been added to our app registration.

---
## Exercise 3: Integrate new Web App into VSTS<a name="ex3"></a>

1. In [VSTS](https://www.visualstudio.com/), open the **Release Definition** that we started in a previous lab. You should be be able to find this by navigating to `Releases` in the `Build & Release` menu on the top navigation. We need to create a second environment to serve as our test web app.

    ![image](./media/2017-06-27_16_46_00.png)

1. Click the drop-down arrow next to the existing Release Definition, and select `Edit`:

    ![image](./media/2017-06-27_16_48_00.png)

1. In the Release Definition, first select `Environment 1`, then select `Add` and select `Clone environment`. We will use our existing Dev web app configuration as the template for the new test web app configuration.

    ![image](./media/2017-06-27_16_49_00.png)

1. Rename the environment from **Copy of...** to **Test** by clicking on it's title.

1. VSTS allows us to control and govern how releases happen between environments. Instead of automatically deploying our test environment after our dev environment, let's add an approval step. A user can look at the dev environment, confirm it is is ready, and then authorize a release to the test environment. Click the `Pre-deployment conditions` icon on the left side of the test environment, select the `After environment` trigger and the first environment:

    ![image](./media/2017-11-03_12_49_00.png)

    For the `Pre-deployment approvers` option, enter your account name and make sure `User requesting a release or deployment should not approve` is **not** checked. Then click the `Save` button:

    ![image](./media/2017-06-27_16_50_00.png)

1.  Switch to the `Tasks` blade. Update the `App service name` to match the web app that you just deployed via the ARM Template. The task now targets the test environment web app, rather than the dev environment web app.

    ![image](./media/2017-06-27_16_52_00.png)

1. Save your Release Definition to finish adding the additional environment.

---
## Exercise 4: Deploy City Power & Light to new Web App<a name="ex4"></a>

With the updated Release Definition, we can now execute a release.

1. Click on the `Release` button and in the drop-down choose `Create Release`.

    ![image](./media/2017-06-27_16_53_00.png)

1. Select a Build to release into the environments. This is likely the largest numbered Build. Then click the `Create` button.

    ![image](./media/2017-06-27_16_55_00.png)

1. Click the Release number to navigate to the Release Details screen:

    ![image](./media/2017-06-27_16_56_00.png)

1. On the top toolbar, select `Logs` to monitor the release process. When the release for the dev environment finishes, you will be prompted to approve the release to the test environment. Click `Approve` to continue the release.

    ![image](./media/2017-06-27_16_57_00.png)

1. Once the test environment app has finished its release, open the app in the browser and login.

    ![image](./media/image-022.gif)

We have now created a new "test" environment web app and app service plan via an ARM Template, and integrated the new environment into our VSTS Release Definition.

---
## Summary

In this hands-on lab, you learned how to:
* Create an ARM Template in Visual Studio Code.
* Deploy ARM Template to Azure via the XPlat CLI.
* Integrate new Web App into VSTS.
* Deploy City Power & Light to new Web App.

After completing this module, you can continue on to Module 6: Monitoring with Application Insights.

### View Module 6 instructions for [Java](../06-appinsights).

---
Copyright 2016 Microsoft Corporation. All rights reserved. Except where otherwise noted, these materials are licensed under the terms of the MIT License. You may use them according to the license as is most appropriate for your project. The terms of this license can be found at https://opensource.org/licenses/MIT.