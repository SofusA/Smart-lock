# Smart-lock
IoT project for course at Technical University of Denmark, 34346 Networking technologies and application development for Internet of Things (IoT).

The system consist of 4 different blocks:¨
* Arduino code for NodeMCU
* Node-RED server, run with Docker
* End-user access point for unlocking, only supports Google Chrome
* End-user access point for changing password

## Getting up and running
### Device
The physical device has to be made, and the Arduino code has to be uploaded to the device. Schematics can be seen below. The HM-10 has to be setup with auto sleep, by sending `AT+PWRM0` with serial.

![alt text](/img/fritzing.png)

### Node-RED server
The server is initialised locally by executing the following command. Note that this is only tested with macOS and Linux systems

```
sh start-docker
```

### Access points
* Node-RED admin panel: `https://127.0.0.1:1880/admin/`
User: `admin`
Password: `gnarlicious`

* Unlock web-app: `https://127.0.0.1:1880`

* Set password: `https://127.0.0.1:1880/setpass`

# Explanation
This section is meant for our supervisors and interested readers.

The system uses two different passwords:
Hidden password, `psw` is hidden from the user, and only exposed to the systems components. `psw` is one time use only, and will be renewed by the device and uploaded to the node-RED server after each use.

Public password is set by the owner of the lock, and shared to the user. When the user inputs the public password into the web-app, the web-app forwards this to the node-RED server, which either responds with the correct `psw` or returns the same password as submitted. Afterwards, the web-app forwards the respond to the device with Bluetooth.

## Device code
### Physical explanation
In short, the device consists of a HM-10 module for bluetooth connection. HM-10 is set to go into sleep mode, with no connection. When a user connects, the device wakes up, and power a relay, which turn on the NodeMCU and the lora module. This allows the device to use very little power. 

### Code 
The code utilises SoftwareSerial library from Arduino. This allows the device to initiate serial communications with digital output pins. This way, all three serial connections can be active at the same time.

To communicate with RN2483, [rn2xx3](https://github.com/jpmeijers/RN2483-Arduino-Library), is used. The library include great examples, and easy methods to send Lora messages.

In short, the code initialises connection with Bluetooth module and Lora module, with `initialize_radio()`, and then wait for message from bluetooth. If the message received matches a secret password, the device unlocks, send confirmation, generates new hidden password and send it to the server with Lora, with `sendGeneratePassword()`.

## Node-RED

The node-RED server has three flows: Parse messages from Lora server, forward hidden password to user, and set new password.
![alt text](/img/node-red.png)

### Parse messages from Lora 
The first flow takes input from the Lora server, in our case: Teracom. Teracom can forward messages with MQTT, so in our case the two servers are connected to a MQTT broker hosted by [cloudMQTT](cloudmqtt.com). When a message is received, it is parsed and stored as a global variable.
In further development, the connection between the two servers should be tcp or similar, and the password should be stored in a database.

### Unlock API
The second flow, creates an API, which the Unlock access point can use to GET the hidden password, if the correct public password is send.

### Set password API
The third flow creates an API which the set password access point can use to change the public password.

## Unlock web-app
The back-end is modified [Web Bluetooth Terminal](https://github.com/loginov-rocks/Web-Bluetooth-Terminal) (MIT Licence), which utilises the library BluetoothTerminal.js by the same author. The library uses [Web Bluetooth  API](https://webbluetoothcg.github.io/web-bluetooth/) by Google. This library is relatively new, and only supports Google Chrome unfortunately.

The web-app serves the purpose of allowing the user to connect to the device, input public password and receive feedback from the device.
The code can be found in `/src/web-app`. Important files are `/src/web-app/index.html`, which is the user interface, built with [Bootstrap](getbootstrap.com), and `/src/web-app/js/main.js` which is the modified main script from Web Bluetooth Terminal.

The two files is programmed to change the colour and text of the button on the site, indicating correct and incorrect passwords. This handled with `terminal.receive()` in `main.js`.

When the user inputs a password in on the web-app and submits, the program fetches the hidden password, with the inputted password from the Unlock API, and sends the hidden password to the device with bluetooth. This is handled with `sendForm.addEventListener()` in `main.js`.

![alt text](/img/unlock.png)

## Set password web-app
The code can be found in `/src/web-app/setpass`.
This web-app is also based on Bootstrap, and just consist of two files: `index.html`, which is draws the website, and `pass.js`, which allows the app to forward the submitted user input to the node-RED server. This is handled with `sendForm.addEventListener()`.

![alt text](/img/setpass.png)