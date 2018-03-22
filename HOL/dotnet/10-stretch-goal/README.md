# Stretch Goal (.NET)

## Overview
City Power & Light is a sample application that allows citizens to report "incidents" that have occurred in their community. It includes a landing screen, a dashboard, and a form for reporting new incidents with an optional photo. The application is implemented with several components:

* Front end web application contains the user interface and business logic. This component has been implemented three times in .NET, NodeJS, and Java.
* WebAPI is shared across the front ends and exposes the backend CosmosDB.
* CosmosDB is used as the data persistence layer.

This lab represents an optional stretch goal exercise where you add an additional feature to the City Power & Light sample application on your own.

## Objectives
In this hands-on lab, you will be using the knowledge gained in [HOL 3](../03-azuread-office365) and learn about some additional Microsoft Graph features. 

## Prerequisites
* Completion of the [HOL 3](../03-azuread-office365).
* A sample solution for this stretch goal is located in the [sample](sample) folder. 

## Exercises
This hands-on-lab has the following exercises:
* [Exercise 1: Create an event](#ex1)

---
## Exercise 1: Create an event<a name="ex1"></a>

In [HOL 3](../03-azuread-office365) you learned about Microsoft Graph and used it to send an email when a new incident has been reported. In this stretch goal you will extend your code to also schedule a meeting for an incident. Your work comprises these tasks:

When an incident is reported: 
* Use Microsoft Graph to read all the current user's contacts.
* Create a new calendar event.
* The event should be scheduled during regular working hours and at least one day in the future.
* As attendees include 3 entries from the user's contact list.
* As body include the details of the incident.

## Hints
* Read these articles on [List contacts](https://developer.microsoft.com/en-us/graph/docs/api-reference/v1.0/api/user_list_contacts) and [Create Event](https://developer.microsoft.com/en-us/graph/docs/api-reference/v1.0/api/user_post_events) from the Microsoft Graph API before you start.
* If you have created a new account to work on the hands-on labs add some sample contacts in [Outlook](https://outlook.office365.com/owa/). Note that real invitations will be send to these contacts when you run your code! If you use your real account analyze the response from the `Contacts` API to pick some contacts that you can send test invitations to.
* Add Microsoft Graph Permissions for your app to read contacts, read and write the calendar at [https://apps.dev.microsoft.com](https://apps.dev.microsoft.com).
* In the `settings.cs` add new constants for Graph URLs to contacts and to events.
* In the `Models` folder create new classes for `Event` to store the values send to the Graph API (similar in use to `Models` -> `MailMessage.cs`) and for `Contact` to deserialize the received data (similar in use to `ViewModels` -> `UserProfileViewModel.cs`). Note that you can use the existing `Body` and `EmailAddress` classes from the `MailMessage` class for your `Event` class and that one class is enough to handle `start` and `end`.
* A response from the `Contacts` API contains more properties than listed in the article above. You will just need the (not listed) `emailAddresses` part. Use the debugger to analyze a response.
* Note that the contacts are contained in an array called `value` that you have to deserialize, too.
* Call your new code in the `Controllers` -> `IncidentController.cs` after `SendIncidentEmail` call in the `Create` method to execute it when a new incident has been reported.
---
## Summary

In this hands-on lab, you learned about some additional Microsoft Graph features and created your own code to extend the City Power & Light sample application.

---
Copyright 2018 Microsoft Corporation. All rights reserved. Except where otherwise noted, these materials are licensed under the terms of the MIT License. You may use them according to the license as is most appropriate for your project. The terms of this license can be found at https://opensource.org/licenses/MIT.