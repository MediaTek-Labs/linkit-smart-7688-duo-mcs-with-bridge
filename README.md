# linkit-smart-7688-duo-mcs-with-bridge
MCS Simple Switch Example with Arduino Bridge Library

# MCS Simple Switch Example with Bridge Library

This example is a simple example that connects LinkIt Smart 7688 Duo to a MCS device with Arduino Bridge library.

Since LinkIt Smart 7688 Duo is somewhat compatible to Arduino **Bridge** library, we can use the bridge library to connect to MCS with Arduino IDE. 

This article shows an example MCS device that can use a ON/OFF switch control to turn on/off the on-board LED of LinkIt Smart 7688.

## Enable the Bridge library
Use following commands to enable Bridge library on LinkIt Smart 7688 Duo:
~~~
uci set yunbridge.config.disabled='0'
uci commit
reboot
~~~
You only need to do this once.

## Setup MCS
Prepare and setup the MCS prototype and test device by following the [MCS Simple Switch tutorial](https://mcs.mediatek.com/resources/latest/tutorial/implementing_using_linkit_one). 

Proceed to **Step 3** in the MCS tutorial to retrieve the device ID, key, and data channel IDs. Unlike the tutorial, which uses LinkIt ONE, we'll be using LinkIt Smart 7688 Duo to connect to MCS.

## Connect LinkIt Smart 7688 Duo to Internet
Please refer to developer's guide to properly setup the Wi-Fi connection of LinkIt Smart 7688 Duo. This provides internet connection and access to MCS web service.

## Code LinkIt Smart 7688 Duo
Use the Arduino IDE to program the 7688 Duo. If you need guide on setting up the Arduino IDE for LinkIt Smart 7688 Duo, refer to developer's guide.

You don't need extra libraries to access MCS. 

To use the sketch:

 * Simply open this sketch(`MCS_with_bridge_http_client.ino`) and upload it to LinkIt Smart 7688 Duo. 
 * Open the **Serial Monitor** - you should see logs like this:
   ~~~
   Query command server:52.77.236.179:443
   connect result = 1
   ~~~
   This means the board has connected to the MCS command server and listens for commands.
 * You can use the [MCS console](https://mcs.mediatek.com/v2console/) to control the ON/OFF of the on-board LED(D13)
    * You should see logs of the command sent from MCS to the board, and The board will then upload the LED status back to MCS to update the `LED Display` channel with logs like this:
    ~~~
    LED_Control,0Switch LED OFF
    POST result:Success.
    ~~~

