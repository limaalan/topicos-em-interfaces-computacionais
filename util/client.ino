/*  This code reads a web-page with pin status in the form of 0 or 1, like: 00100010
 *  It acts as a web client and activates its coresponding pins. See the server code...
 *  I chose to monitor Analog pins A0-A5 because most of the digital pins are taken over by the Ethernet shield
 *  =========================================================
 *  Viorel Racoviteannu /
 *  https://www.youtube.com/@Racov
 *  https://racov.ro
 *  YO3RAK@gmail.com
 *  =========================================================
 *  I cannot take any responsibility for missuse of this code or any kind of damage it may occur from using this code.
 *  =========================================================
 *  ---Version history---
 *  Jul. 2023 - first try :)
*/

// UIPEthernet library is used for Arduino Nano Ethernet Shiel
// It is not needed if you are using Arduino UNO/Duemilanove/Mega/etc.
#include <UIPEthernet.h>

// **** ETHERNET SETTING ****

byte mac[6] = {0x01,0x02,0x03,0x04,0x05,0x07};
IPAddress ip(192, 168, 1, 116);
EthernetClient client;
signed long next;

void setup() {
  // pin declaration
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);

  Serial.begin(9600);
// display splash screen:
  Serial.println("Viorel Racoviteannu / Jul. 2023");
  Serial.println("https://www.youtube.com/@Racov");
  Serial.println("https://racov.ro");
  Serial.println("YO3RAK@gmail.com");
  Serial.println("Reads a web-page with pin status in the form of 0 or 1, like: 001001");
  Serial.println("It acts as a web client and activates its coresponding pins");
  Serial.println("I chose to monitor Analog pins A0-A5 because most of the digital pins are taken over by the Ethernet shield \n\r");
  digitalWrite(A0, LOW);                  // trigger some sort of alarm
  digitalWrite(A1, LOW);                  // for COMM FAIL
// start the Ethernet connection
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.print("local IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("");

  next = 0;
}

void loop() {

  if (((signed long)(millis() - next)) > 0)
    {
      next = millis() + 5000;
      Serial.print("Connecting to server ->");
      if (client.connect(IPAddress(192, 168, 1, 115),80))
        {
          Serial.print("-> Connected. ");
          client.println("GET HTTP/1.1");
          client.println("Connection: close");
          client.println();
          while(client.available()==0)
            {
              if (next - millis() < 0)
                goto close;
            }
          int size;
          while((size = client.available()) > 0)
            {
              String result = client.readString();
              Serial.print("Received-> ");
              //Serial.print(result);               // print the entire message for debuging
              for (int i = 42; i <= 47; i++) {      // print only the important bits (A0->A5)
                Serial.print(result.charAt(i));
              }
              // extracting each bit and activating the corresponding pin
              char val1; int val2;
              val1 = result.charAt(42);             // extract the pin value of the (remote) server
                val2 = val1 - '0';                  // conversion from 'char' to 'int'
                digitalWrite(A0, val2);             // activate the (local) client respective pin
              val1 = result.charAt(43);
                val2 = val1 - '0';
                digitalWrite(A1, val2);
              val1 = result.charAt(44);
                val2 = val1 - '0';
                digitalWrite(A2, val2);
              val1 = result.charAt(45);
                val2 = val1 - '0';
                digitalWrite(A3, val2);
              val1 = result.charAt(46);
                val2 = val1 - '0';
                digitalWrite(A4, val2);
              val1 = result.charAt(47);
                val2 = val1 - '0';
                digitalWrite(A5, val2);
            }

          close:
          //disconnect client
          Serial.println(" <-Disconnected.");
          client.stop();
        }
      else {
        Serial.println(" <-Connection failed.");
      }
    }
}
