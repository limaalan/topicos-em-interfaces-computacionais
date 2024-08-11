/*  This code outputs a web-page with pin status in the form of 0 or 1, like: 00100010
 *  It acts as a web server.
 *  I chose to monitor Analog pins A0-A5 because most of the digital pins are taken over by the Ethernet shield
 *  =========================================================
 *  Viorel Racoviteannu /
 *  https://www.youtube.com/@Racov
 *  https://racov.ro
 *  YO3RAK@gmail.com
 *  =========================================================
 *  I cannot take any responsibility for missuse of this code or any kind of damage it may occur from using this code.
 *  =========================================================
 ---Version history---
 *  Jul. 2023 - first try :)
*/

// UIPEthernet library is used for Arduino Nano Ethernet Shiel
// It is not needed if you are using Arduino UNO/Duemilanove/Mega/etc.
#include <UIPEthernet.h> // Used for Ethernet

// **** ETHERNET SETTING ****
// byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE };
byte mac[] = {0x01,0x02,0x03,0x04,0x05,0x06};
IPAddress ip(192, 168, 1, 115);
EthernetServer server(80);

char Pin[9]="";        // string to keep the pin status
char WebWrite[40]="";  // string to be displayed on the web
int val;

void setup() {
  // pin declaration
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  Serial.begin(9600);
 // display splash screen:
  Serial.println("Viorel Racoviteannu / Jul. 2023");
  Serial.println("https://www.youtube.com/@Racov");
  Serial.println("https://racov.ro");
  Serial.println("YO3RAK@gmail.com");
  Serial.println("This code creates a web-page with pin status in the form of 0 or 1, like: 00100010");
  Serial.println("I chose to monitor Analog pins A0-A5 because most of the digital pins are taken over by the Ethernet shield \n\r");
 // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  delay(1000);
  server.begin();
  Serial.print("local IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println(" \n\r");
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) 
  {  
    Serial.print("New Connection. ");
    Serial.print("Sending-> ");
    Serial.print(Pin);
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    while (client.connected()) 
    {
      // read the status of the pins and create the string
      val = digitalRead(A0);
      if(val == 0 ) { Pin[0] = '0'; }
        else { Pin[0] = '1'; }
      val = digitalRead(A1);
      if(val == 0 ) { Pin[1] = '0'; }
        else { Pin[1] = '1'; }
      val = digitalRead(A2);
      if(val == 0 ) { Pin[2] = '0'; }
        else { Pin[2] = '1'; }
      val = digitalRead(A3);
      if(val == 0 ) { Pin[3] = '0'; }
        else { Pin[3] = '1'; }
      val = digitalRead(A4);
      if(val == 0 ) { Pin[4] = '0'; }
        else { Pin[4] = '1'; }
      val = digitalRead(A5);
      if(val == 0 ) { Pin[5] = '0'; }
        else { Pin[5] = '1'; }
      // creating the line with pin values
      strcpy (WebWrite ,"<body><h3>");
      strcat (WebWrite , Pin);
      strcat (WebWrite , "</h3></body>");
      if (client.available()) 
      {
        char c = client.read();

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          client.print("<html><title>Web Control</title>");
          client.println(WebWrite);
          client.println("<body><h3>A0=>A5</h3></body>");
          client.println("<body><h3>Viorel Racoviteanu 2023</h3></body>");
          client.println("<body><h3>https://racov.ro || https://www.youtube.com/@Racov</h3></body>");
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    // give the web browser time to receive the data
    delay(100);

    // close the connection:
    client.stop();
    Serial.println(" <- Disconnected");
  }
}