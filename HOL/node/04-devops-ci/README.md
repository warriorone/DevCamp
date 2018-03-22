# DevOps with Visual Studio Team Services (NodeJS)

## Overview
In this lab, you will create a Visual Studio Team Services online account, check in your code, create a Continuous Integration pipeline, and test your cloud-based application.

## Objectives
In this hands-on lab, you will learn how to:
* Create a Visual Studio Team Services online account.
* Create a VSTS Git repository.
* Add your code to the VSTS Git repository.
* Create a Continuous Integration pipeline.
* Deploy a built application to an Azure Web App from VSTS.

## Prerequisites

* The source for the starter app is located in the [start](start) folder. 
* There will be no code changes required so the the [end](end) folder will remain empty. 
* Deployed the starter ARM Template [HOL 1](../01-developer-environment).
* Completion of the [HOL 3](../03-azuread-office365).

> &#x1F53A; **Note**: If you did not complete the previous labs, the project in the [start](start) folder is cumulative. But you need to add the previous HOL's environment variables and make all necessary changes to Azure. &#x1F53A;

## Exercises
This hands-on-lab has the following exercises:
* [Exercise 1: Create VSTS online account](#ex1)
* [Exercise 2: Create VSTS Git repository](#ex2)
* [Exercise 3: Add application to VSTS Git](#ex3)
* [Exercise 4: Create a Continuous Integration pipeline](#ex4)
* [Exercise 5: Deploy code to an Azure Web App](#ex5)

---
## Exercise 1: Create VSTS online account<a name="ex1"></a>

> &#x1F53A; You can skip this exercise if you already created an account in HOL 1. &#x1F53A;

1. In your browser, navigate to [https://www.visualstudio.com/](https://www.visualstudio.com/)

    ![image](./media/2017-06-21_13_14_00.png)

1. Click `Get started for free` link below `Visual Studio Team Services`.

1. Log in with your Azure AD account.

1. You will be asked to choose an hosting address and if you want to use Git or Team Foundation Version Control. Select `Git` and click `Continue`.

1. A new project called `MyFirstProject` will be automatically created.

---
## Exercise 2: Create VSTS Git repository<a name="ex2"></a>

VSTS gives us the option to use Git or [TFVC](https://www.visualstudio.com/en-us/docs/tfvc/overview) as our project's repository. For this exercise we will use Git, and then clone the repository to our dev machine. 

> Note that if you acquired these lab materials via a `git clone` of the workshop repo then you should select a folder somewhere else on your dev machine. This will minimize conflicts between the two separate repositories.

1. We will ignore the automatically created `MyFirstProject`. Starting at your TFVC account's landing page, click `New Project`.

    ![image](./media/2017-06-21_13_56_00.png)

1. Enter a project name such as **DevCamp**, ensure `Version control` is set to `Git` and then click `Create`.

    ![image](./media/2017-06-21_13_58_00.png)

1. Wait for the project to be created. This process may take up to 60 seconds. When finished you will be redirected to the project page

1. Click `Dashboards` and explore your pre-built dashboard. Familiarize yourself with the variety of widgets available, and the customization options. 

    ![image](./media/2017-06-21_14_01_00.png)

You have now created a project in VSTS with a Git repository. Next we'll clone the repository locally to your developer machine and upload code from our machine to VSTS.

---
## Exercise 3: Add application to VSTS Git<a name="ex3"></a>

1. Click `Code` on the top toolbar to navigate to the Code screen. Then click the `Generate Git Credentials` button to set a user name, alias, and password.

    ![image](./media/2017-06-26_16_46_00.png)

1. Next, select the `Copy` icon to copy the HTTPS URL for the repository.

1. In a console window, navigate to a spot on your dev machine and execute a `git clone https://yourrepo.com/DefaultCollection/_git/Repo.git`

    ![image](./media/2017-06-26_16_52_00.png)

    Depending on your environment setup you may need to authenticate with VSTS.


1. When we cloned our repository it was empty. Take the code that you have developed in the earlier labs (or the `start` folder bundled with this readme) and paste it into our new directory. This can be done via the command line, or with good old copy/paste in an Explorer or Finder window.

    ![image](./media/2017-06-27_09_03_00.png)

    > Depending on how your environment is setup, there may be a hidden folder `.git` in your originating directory. Do **not** copy this folder into the destination directory linked to VSTS.

1. Back in the console, execute a `git status` to ensure the files are picked up by git.

    ```CMD
    git status
    ```

    ![image](./media/2017-06-27_09_06_00.png)

1. Execute `git add *` to track the files, then a `git commit -m "initial upload"` to commit the files to the repository. Finally, execute `git push origin master` to push the files up to VSTS.

    ```CMD
    git add *
    git commit -m "initial upload"
    git push origin master
    ```

    ![image](./media/2017-06-27_09_10_00.png)

    > You might have to provide information before you can make your first commit. In this case execute these lines with your login data and try again:
    
    ```CMD
    git config --global user.email "your@email.com"
    git config --global user.name "Your name"
    ```

1. In the browser, reload the `Code` page to see the uploaded code:

    ![image](./media/2017-06-27_09_15_00.png)

1. Now, any changes you make to the local repository can be pushed up to VSTS. Other team members may also begin interacting with the code base via their own clones and pushes.

    > Note that we did not include the `node_modules` or `.vscode` folders. These components are typically not added to source control, as they bloat the size of the repository. These files should have been excluded from your repository due to settings in the `.gitignore` file.

---
## Exercise 4: Create a Continuous Integration pipeline<a name="ex4"></a>

With application code now uploaded to VSTS, we can begin to create builds via a Build Definition. Navigate to the `Build` tab from the top navigation. We will use the hosted agent within VSTS to process our builds in this exercise.

1. From the `Build & Release` tab, create a new `Build Definition` by clicking the `New Definition` button:

    ![image](./media/2017-06-21_14_35_00.png)

1. There are prebuilt definitions for a variety of programming languages and application stacks. For this exercise select `Empty` and click `Apply`:

    ![image](./media/2017-06-27_09_21_00.png)
    
1. Use the suggested name and select the `Default` from the `Default agent queue` drop-down.

    ![image](./media/2017-08-11_09_08_00.png)

1. The build tasks are created for us as part of the template. 

1. On the `Get sources` step set the `From` value to your VSTS Project and the `Repository` to the repo that was earlier created.

    ![image](./media/2017-06-27_09_23_00.png)

1. After the empty Build Definition is created, we need to create a series of Build Steps.

    * Verify NodeJS version installed on the build agent by echoing it to the console.
    * Restore all package dependencies with `npm install`.
    * Package the code assets into a deployable zip file.
    * Publish the zip file as a Publish Artifact that can be consumed by the VSTS Release System.

    Each of these steps will begin by clicking the `Add Task` button.

1.  Add a Build Step for `Command Line`, found under the filter for `Utility`. Click `Add` and then select the newly added task to configure it.

    ![image](./media/2017-06-30_11_38_00.png)
    
    * Change the name of the build step to `Echo Node Version`.
    * Change the value `Tool` to `node` and the value `Argument` to `-v`.
    
    ![image](./media/2017-06-30_11_39_00.png)

1.  Add a Build Step for `npm`, found under the filter for `Package`. Click `Add` and then select the newly added task to configure it.

    ![image](./media/2017-06-30_11_43_00.png)
    
    * Change the name of the build step to `Install Dependencies`.
    * Change the value `Command` to `install`.
    
    ![image](./media/2017-06-30_11_44_00.png)

    > If you are using `devDependencies` in your `package.json` and want to control whether or not they are installed, then pass the `--production` flag in the `Argument` field.

1.  Add a Build Step for `Archive`, found under the filter for `Utility`. Click `Add` and then select the newly added task to configure it.

    ![image](./media/2017-06-30_11_46_00.png)
    
    In configuration boxes, we can use variables in addition to string literals.
    
    * Configure `Root Folder` to use the directory on the Build Agent that contains our sources files by inserting `$(Build.SourcesDirectory)`. 
    * For `Archive file to create` insert `$(Build.SourcesDirectory)/archive/$(Build.BuildId).zip`. This will dynamically name our zip file of code with the build number.
    * Uncheck the box for `Prefix root folder name to archive paths` to avoid an unnecessary nesting within the .zip file.

    > You can define your own variables to use throughout the Build and Release pipelines by clicking `Variables` in the Build Definition's sub-navigation. Also see [here](https://www.visualstudio.com/docs/build/define/variables) for all pre-defined variables available.
    
    ![image](./media/2017-06-30_11_49_00.png)

1. Finally, a Build Step for `Publish Build Artifacts`, found under the filter for `Utility`. Click `Add` and then select the newly added task to configure it. This step outputs a file(s) from our Build Definition as a special "artifact" that can be used in VSTS' Release Definitions.

    ![image](./media/2017-06-30_11_58_00.png)
    
    * Change the value `Path to Publish` to `$(Build.SourcesDirectory)/archive/$(Build.BuildId).zip` to target the zip file created in the previous Build Step.
    * Change the value `Artifact Name` to `drop`.
    * Change the value `Artifact publish location` to `Visual Studio Team Services/TFS`.

    ![image](./media/2017-06-30_12_02_00.png)

1. Click `Save & queue`. Our saved Build Definition is ready to be processed by the Hosted Build Agent.

    ![image](./media/2017-06-27_10_10_00.png)
    
    ![image](./media/2017-06-30_12_09_00.png)

1. Accept the defaults and click `Queue`. Your build will then be queued until the Hosted Build Agent can pick it up for processing. This typically takes less than 60 seconds to begin.

1. Once your build completes, click each step on the left navigation bar and inspect the output.

    ![image](./media/2017-06-27_10_15_00.png)

1. Once your build completes, click each step on the left navigation bar and inspect the output. For `Echo Node Version` we can see the agent's version in the right `Logs` pane.

    ![image](./media/2017-06-30_12_22_00.png)

1. Let's inspect the output artifacts that were published. Click the `Build XXX` header in the left pane to view the build's landing page. 

    ![image](./media/2017-06-30_12_24_00.png)
    
1. Select `Artifacts` from the horizontal toolbar.

1. A `drop` folder has been created containing the compiled output. Click `Explore` to see them.

1. Expand the `drop` folder and view the build artifacts. Click `Close` when complete.

1. Click `Download` next to the `drop` folder to save the build locally.

1. Unzip `drop.zip` to see the application files created by the build agent (including the restored `node_modules` folder). This artifact will be deployed to an Azure Web App in a later exercise.

    ![image](./media/2017-06-30_12_32_00.png)

We now have a Build Definition that will construct our NodeJS application and package it for deployment anytime code is checked into the repository, or a manual build is queued.

---
## Exercise 5: Deploy code to an Azure Web App<a name="ex5"></a>

In the ARM Template that was originally deployed, a web app was created as a development environment to hold a deployed NodeJS application. We will use this web app as a deployment target from VSTS. First, we need to prepare this web app for our application code.

1. Visit the Azure Web App by browsing to the [Azure Portal](http://portal.azure.com), opening the `DevCamp` Resource Group, and select the Azure Web App resource that begins with `nodejsapp` before the random string. 

    ![image](./media/2017-06-30_13_00_00.png)

1. Once the blade expands, select `Browse` from the top toolbar:

    ![image](./media/2017-06-30_13_01_00.png)

1. A new browser tab will open with a splash screen visible. It will look similar to this image (it gets updated regularly with new information) and tell you that the app service has been created:

    ![image](./media/2017-06-30_13_02_00.png)

1. We can deploy our code to this Azure Web App, however it was not configured with our AzureAD details. When trying to authenticate, AzureAD would refuse since it does not know about this domain. 

    To fix this, return to the [Application Registration Portal](https://apps.dev.microsoft.com), login, and open your application settings.

    In the section for `Platforms`, click **Add Url** to add the URL of your Azure Web App from Step 1. Remember to append the `/auth/openid/return` route at the end, since that is the route that will process the return data from AzureAD. Ensure this address is using **https**.

    ![image](./media/2017-06-30_13_05_00.png)

    Make sure you click `Save` at the bottom of the screen to add the URL to your AzureAD app.

1. Now that AzureAD is configured, we need to add our AzureAD related environment variables to the Azure Web App. Back in the ***nodejsapp...*** blade where you hit `Browse` earlier, open `Application Settings` from the left navigation.

    Find the `App Settings` section containing a table of settings. In the ARM Template we auto-generated the majority of these settings, however we need to add a few additional environment variables to match the `.vscode/launch.json` file that we have been using locally.

    * `AAD_RETURN_URL` should be set to the same URL that we just configured for our AzureAD application. Should be similar to `https://nodejsapp....azurewebsites.net/auth/openid/return`. Ensure this is using **https**.

    * `AAD_CLIENT_ID` should match the Application ID in the [Application Registration Portal](https://apps.dev.microsoft.com) and similar to `2251bd08-10ff-4ca2-a6a2-ccbf2973c6b6`.

    * `AAD_CLIENT_SECRET` should be the Application Secret generated in the apps portal, and be similar to `JjrKfgDyo5peQ4xJa786e8z`.

    ![image](./media/2017-06-30_13_31_00.png)
    
    After you have created the new settings, click `Save`.

1. Now that we have a build being created and a website to deploy into, let's connect them. Back in our VSTS Build Definition, navigate to the `Build & Release` tab.

1. Click on `Releases`.

1. Click the `+ New Definition` button to create a new release definition:

    ![image](./media/2017-06-22_11_31_00.png)

1. Select `Empty` and click `Apply`.

    ![image](./media/2017-06-27_11_23_00.png)

1. Click `Add artifact`, select the build definition you created as the `Source (Build definition)` and click `Add`. Now the artifacts the build definition created will be available in the new release definition.

    ![image](./media/2017-06-27_11_24_00.png)

1. Switch to `Tasks` and click the `+` button.

    In the task catalog, find `Azure App Service Deploy` and click `Add`.

    ![image](./media/2017-06-27_11_33_00.png)

1. Click on the task name `Azure App Service Deploy` to open the attributes for that task. VSTS will need a connection to the Azure subscription that you are deploying the application to.

1. If the drop-down next to `Azure subscription` offers you your subscription, select it, authorize it and continue to select your `nodejsapp...` Azure Web app resource from the `App Service name` drop-down.
 
1. If the drop-down next to `Azure subscription` does not offer you your subscription or the drop-down next to `App Service name` does not offer you your Azure Web app resource (give it a moment after selecting the subscription), click on `Manage` to open a new browser tab holding configuration options.

    ![image](./media/2017-06-27_13_13_00.png)
    
    1. In the new tab, select `New Service Endpoint` and from the drop-down choose `Azure Resource Manager`.

    ![image](./media/image-041.gif)

    2. The modal window should automatically determine your subscription information. Provide a name such as **Azure**, select `OK`, and a close the browser tab.

    ![image](./media/image-042.gif)

    3. If your subscription is not in the drop-down list, click the link at the bottom of the window, and the window format will change to allow you to enter connection information on your subscription:    

    ![image](./media/image-043.gif)

    4. If you have not created a service principal for the subscription, you will have to follow the 
    [instructions](https://go.microsoft.com/fwlink/?LinkID=623000&clcid=0x409) to do so. This process will provide the information to enter in this dialog:
    
        1. Open [this PowerShell script](https://raw.githubusercontent.com/Microsoft/vsts-rm-documentation/master/Azure/SPNCreation.ps1) in your browser.
        1. Select all the content from the window and copy to the clipboard.
        1. Open a PowerShell ISE window.
        1. In the text window, paste the PowerShell script from the clipboard.

    ![image](./media/image-044.gif)

    5. Click the green arrow to run the PowerShell script.

    ![image](./media/image-045.gif)

    6. The PowerShell script will ask for your `subscription name` and a `password`. This password is for the service principal only, not the password for your subscription. So you can use whatever password you would like, just remember it.    

    ![image](./media/image-046.gif)

    7. You will then be asked for your Azure login credentials. Enter your Azure username and password. The script will print out several values that you will need to enter into the `Add Azure Resource Manager Service Endpoint` window. Copy and paste these values from the PowerShell window:
    
        * Subscription ID
        * Subscription Name
        * Service Principal Client ID
        * Service Principal Key
        * Tenant ID
        
    Also, enter a user-friendly name to use when referring to this service endpoint connection.

    ![image](./media/image-047.gif)

    Click `Verify connection`, and ensure that the window indicates that the connection was verified. Then Click `OK` and `Close`.

    ![image](./media/image-048.gif)

    > This pattern is used to connect to a variety of services beyond Azure such as Jenkins, Chef, and Docker.

1. Back on the VSTS Build window, in the Build Step we started earlier, click the `Refresh` icon. The **Azure** connection that we setup should now appear.  Select it. 

    ![image](./media/2017-06-27_13_31_00.png)

    Next, for **App Service Name** click the `Refresh` icon, choose the name of the NodeJS Azure Web App. It may take a moment to populate.

    ![image](./media/2017-06-27_11_42_00.png)

1. `Save` the Build Definition, accepting the default folder and supplying a comment if you'd like. Next, click `Release` and `Create Release`:

    ![image](./media/2017-06-27_11_57_00.png)

    In the following window, type a description for the release and click `Create`:

    ![image](./media/2017-06-27_11_59_00.png)

    You should see a message that your release has been created. Click on the link for `Release-1`.

    ![image](./media/2017-06-27_12_06_00.png)
    
    You will see the status for the release:

    ![image](./media/2017-06-27_12_07_00.png)
    
    When the release is complete, browse to the deployment website. You should see the same application you tested in the modern-apps lab:

    ![image](./media/2018-01-18_13_42_00.jpg)

    If you make changes to the application and `git push` back to the VSTS server, this will automatically trigger a build and deployment. Try to make a small change to the application and verify that the application is re-deployed to the test environment.

--- 
## Summary

In this hands-on lab, you learned how to:
* Create a Visual Studio Team Services online account that you used for version control of your code, automatic compiling and deploying of your web app.
* Create a VSTS Git repository that you utilized to synchronize your source code on your machine and in the cloud.
* Add your code to the VSTS Git repository.
* Create a Continuous Integration pipeline that you used to automatically compile your application and create packages for deployment anytime code is checked into the repository.
* Deploy a built application to an Azure Web App from VSTS and thus automating the final steps of your deployment process.


After completing this module, you can continue on to Module 5: ARM.

### View instructions for [Module 5 for NodeJS](../05-arm-cd).

---
Copyright 2016 Microsoft Corporation. All rights reserved. Except where otherwise noted, these materials are licensed under the terms of the MIT License. You may use them according to the license as is most appropriate for your project. The terms of this license can be found at https://opensource.org/licenses/MIT.