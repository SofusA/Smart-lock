/*
   Connect the RN2xx3 as follows:
   RN2xx3 -- ESP8266
   Uart TX -- D2
   Uart RX -- D1
   Reset -- D8
   Vcc -- 3.3V
   Gnd -- Gnd

   Connect Bluetooth:
   RX -- D4
   TX -- D3
*/


int ledPin = 2; //built in LED
int buttonPin = 0; // Build in button

int psw; // Password int

#include <rn2xx3.h>
#include <SoftwareSerial.h>

// RN2483 stuff
SoftwareSerial RN2483Serial(D2, D1); // RX, TX !! labels on relay board is swapped !!
#define RESET D8
rn2xx3 myLora(RN2483Serial);
const char *appEui = "BE7A000000001158";
const char *appKey = "441A1EACD8F19470A2AA6D84EC887D21";

// Bluetooth stuff
SoftwareSerial BTserial(D6, D5); // RX, TX

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  randomSeed(analogRead(0));

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  RN2483Serial.begin(57600);
  BTserial.begin(9600);

  delay(5000); //wait for the arduino ide's serial console to open

  initialize_radio();
}

void loop() {
  if (digitalRead(buttonPin) == 0) {
    Serial.println("Button Pressed");
    psw = sendGeneratePassword();
    Serial.print("New password: ");
    Serial.println(psw);
    delay(500);
  }

}

void initialize_radio() {
  //reset RN2xx3
  pinMode(RESET, OUTPUT);
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

  while (!join_result) {
    Serial.println("Unable to connect");
    delay(30000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully connected to gateway");
}

void led_on() {
  digitalWrite(ledPin, 0);
}

void led_off() {
  digitalWrite(ledPin, 1);
}

void ledBlink() {
  digitalWrite(ledPin, 0);
  delay(100);
  digitalWrite(ledPin, 1);
  delay(100);
  digitalWrite(ledPin, 0);
  delay(100);
  digitalWrite(ledPin, 1);
  delay(100);
  digitalWrite(ledPin, 0);
  delay(100);
  digitalWrite(ledPin, 1);
}

int sendGeneratePassword() {
  int newpsw;
  newpsw = random(1000, 9999);
  ledBlink();
  myLora.tx(String(newpsw));

  return newpsw;
}
