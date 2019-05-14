# Smart-lock
IoT project for course at Technical University of Denmark, 34346 Networking technologies and application development for Internet of Things (IoT).

The system consist of 4 different blocks:¨
* Arduino code for NodeMCU
* Node-red server, run with Docker
* End-user access point for unlocking, only supports Google Chrome
* End-user access point for changing password

## Getting up and running
### Device
The physical device has to be made, and the Arduino code has to be uploaded to the device. Schematics can be seen below.

### Node-red server
The server is initialised locally by executing the following command. Note that this is only tested with macOS and Linux systems

```
sh start-docker
```

### Access points
* Node-red admin panel: `https://127.0.0.1:1880/admin/`
User: `admin`
Password: `gnarlicious`

* Unlock: `https://127.0.0.1:1880`

* Set password: `https://127.0.0.1:1880/setpass`



Shamelessly stolen web bluetooth terminal https://github.com/loginov-rocks/Web-Bluetooth-Terminal