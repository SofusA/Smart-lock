/*
   Connect the RN2xx3 as follows:
   RN2xx3 -- ESP8266
   Uart TX -- D2
   Uart RX -- D1
   Reset -- D4
   Vcc -- 3.3V
   Gnd -- Gnd

   Connect Bluetooth:
   RX -- D6
   TX -- D5
*/

int lockPin = D7; //Unlock relay

String btMessage; //For storing last BT message

int psw; // Password int

// RN2483 stuff
#include <rn2xx3.h>
#include <SoftwareSerial.h>

SoftwareSerial RN2483Serial(D2, D1); // Initialise serial with RN2483
#define RESET D4 // Reset on RN2483 
rn2xx3 myLora(RN2483Serial); // Define lora object for rn2xx3 library
const char *appEui = "BE7A000000001158"; //appEui for lora network
//const char *appKey = "6F32DC311799944F3ED1164A20371695";
const char *appKey = "441A1EACD8F19470A2AA6D84EC887D21"; //appKey for lora network

// Bluetooth stuff
SoftwareSerial BTserial(D5, D6); // Initialise serial with HM-10o

void setup() {
  pinMode(lockPin, OUTPUT);
  pinMode(RESET, OUTPUT);
  digitalWrite(lockPin, 1);

  randomSeed(analogRead(0)); // Generate random seed based on random input from analog pin

  initialize_radio();
}

void loop() {
  if (BTserial.available()) { // If incoming message do:
    btMessage = BTserial.readString(); // Save message
    Serial.print(btMessage);  // Print the message

    if (btMessage.startsWith("OK+")) { // Ignore messages from HM-10, we are only interessted in messages from user
      return;
    } else if (btMessage.toInt() == psw) { // If message matches psw do:
      BTserial.println("Unlocked"); // Send "Unlocked" back to the user
      digitalWrite(lockPin, 0);     // Unlock
      psw = sendGeneratePassword(); // Send PSW to server and generate new psw
      delay(10000);                 // Wait to allow user to open box
      BTserial.println("Generated new psw");  // Send confirmation to user
      digitalWrite(lockPin, 1);     // Lock device
    } else {
      BTserial.println("Incorrect password"); // Send incorrect password if psw did not match
    }
  }
}


void initialize_radio() {
  Serial.begin(9600);       // Begin serial with Arduino IDE, if connected to computer
  RN2483Serial.begin(57600);// Begin serial with Lora module
  BTserial.begin(9600);     // Begin serial with bluetooth module

  //reset RN2xx3
  digitalWrite(RESET, LOW); // Reset Lora. Note that NodeMCU has inverted output compared to UNO
  delay(100);
  digitalWrite(RESET, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  RN2483Serial.flush();
  myLora.autobaud(); // Autobaud the rn2483 module to 9600. The default would otherwise be 57600.


  //check communication with radio
  String hweui = myLora.hweui(); // Store hardware EUI
  while (hweui.length() != 16)  { // Attempt communication with Lora Module
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  // print out the HWEUItl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());


  Serial.println("Trying to connect to gateway");
  bool join_result = false;

  //OTAA: initOTAA(String AppEUI, String AppKey);
  join_result = myLora.initOTAA(appEui, appKey);   // Join the network

  while (!join_result) { // Try again if failed
    Serial.println("Unable to connect");
    delay(10000); //delay a minute before retry
    join_result = myLora.init();
  }

  Serial.println("Successfully connected to gateway");

  psw = sendGeneratePassword(); // generate and send a PSW to the server
}

int sendGeneratePassword() { // generate and send a PSW to the server
  int newpsw;
  newpsw = random(1000, 9999); // Genreate psw
  myLora.tx(String(newpsw));   // Send it with lora

//  Serial.print("New password: ");
//  Serial.println(newpsw);

  return newpsw; // Return the generated psw
}
