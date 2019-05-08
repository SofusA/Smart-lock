#include <SoftwareSerial.h>
SoftwareSerial BTserial(D4, D3); // RX, TX

const long baudRate = 9600;
char c=' ';
String psw_in = "";
boolean NL = true;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Sketch: "); Serial.println(__FILE__);
  Serial.print("Uploaded: "); Serial.println(__DATE__);
  Serial.println(" ");
  pinMode(D6, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D1, OUTPUT);
  BTserial.begin(baudRate);
  Serial.print("BTserial started at ");
  Serial.println(baudRate);
  Serial.println(" ");
}

void loop() {

  // Read from the Bluetooth module and send to the Arduino Serial Monitor
  /*if (BTserial.available()){
      c = BTserial.read();
      Serial.write(c);
      if(c == 'L'){
        digitalWrite(D1, LOW);
        digitalWrite(D2, HIGH);
        }
       if(c == 'H'){
        digitalWrite(D1, HIGH);
        digitalWrite(D2, LOW);
        }
   }*/
   digitalWrite(D2, HIGH);
   if(BTserial.available()){
    psw_in = BTserial.readString();
    Serial.println(psw_in);
     if(psw_in == "open"){
      digitalWrite(D2, LOW);
      digitalWrite(D1, HIGH);
      delay(2000);
      digitalWrite(D1, LOW);
      digitalWrite(D2, HIGH);
      }
   }
      
  // Read from the Serial Monitor and send to the Bluetooth module
  if(Serial.available()){
    c = Serial.read();
    BTserial.write(c);
      
    // Echo the user input to the main window. The ">" character indicates the user entered text.
    if (NL) {
      Serial.print(">");
      NL = false;
     }
     Serial.write(c);
     if (c==10) {
      NL = true;
     }
  }
}
