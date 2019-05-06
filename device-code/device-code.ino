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

SoftwareSerial RN2483Serial(D2, D1); // RX, TX !! labels on relay board is swapped !!
#define RESET D4
rn2xx3 myLora(RN2483Serial);
const char *appEui = "BE7A000000001158";
//const char *appKey = "6F32DC311799944F3ED1164A20371695";
const char *appKey = "441A1EACD8F19470A2AA6D84EC887D21";

// Bluetooth stuff
SoftwareSerial BTserial(D5, D6);

void setup() {
  pinMode(lockPin, OUTPUT);
  pinMode(RESET, OUTPUT);
  digitalWrite(lockPin, 1);

  randomSeed(analogRead(0));

  initialize_radio();
}

void loop() {
  //  if (digitalRead(buttonPin) == 0) {
  //    Serial.println("Button Pressed");
  //    psw = sendGeneratePassword();
  //    Serial.print("New password: ");
  //    Serial.println(psw);
  //    delay(500);
  //  }

  if (BTserial.available()) {
    btMessage = BTserial.readString();
    Serial.print(btMessage);

    if (btMessage.startsWith("OK+")) {
      return;
    } else if (btMessage.toInt() == psw) {
      BTserial.println("Unlocked");
      digitalWrite(lockPin, 0);
      psw = sendGeneratePassword();
      delay(10000);
      BTserial.println("Generated new psw");
      digitalWrite(lockPin, 1);
    } else {
      BTserial.println("Incorrect password");
    }
  }
}


void initialize_radio() {
  Serial.begin(9600);
  RN2483Serial.begin(57600);
  BTserial.begin(9600);

  delay(5000); //wait for the arduino ide's serial console to open

  //reset RN2xx3
  digitalWrite(RESET, LOW);
  delay(100);
  digitalWrite(RESET, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  RN2483Serial.flush();

  // Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while (hweui.length() != 16)  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to connect to gateway");
  bool join_result = false;

  //OTAA: initOTAA(String AppEUI, String AppKey);
  join_result = myLora.initOTAA(appEui, appKey);

  //  while (!join_result) {
  //    Serial.println("Unable to connect");
  //    delay(10000); //delay a minute before retry
  //    join_result = myLora.init();
  //  }

  Serial.println("Successfully connected to gateway");

  psw = sendGeneratePassword();
}

int sendGeneratePassword() {
  int newpsw;
  newpsw = random(1000, 9999);
  //  myLora.tx(String(newpsw));

  Serial.print("New password: ");
  Serial.println(newpsw);
  
  BTserial.print("New password: ");
  BTserial.println(newpsw);

  return newpsw;
}
