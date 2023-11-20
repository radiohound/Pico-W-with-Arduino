
/*
  Pico W Web Interface Demo
  picow-web-control-demo.ino
  Web Interface & WiFi Connection
  Control the onboard LED with Pico W
 
  Modified example from 
 
  DroneBot Workshop 2022
  https://dronebotworkshop.com/picow-arduino/

  I found this helpful for adding backslashes to the '"' in html code, 
  so that it can be handled correctly with the Serial.println function.
  See https://www.freecodeformat.com/add-backslash.php to convert pieces of HTML code.
*/
 
// Load Wi-Fi library
#include <WiFi.h>
 
// Replace with your network credentials
const char* ssid = "YourSSIDHere";
const char* password = "yourPassword";
 
// Set web server port number to 80
WiFiServer server(80);
 
// Variable to store the HTTP request
String header;
 
// Variable to store onboard LED state
String picoLEDState = "off";
 
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
 
void setup() {
 
  // Start Serial Monitor
  Serial.begin(115200);
 
  // Initialize the LED as an output
  pinMode(LED_BUILTIN, OUTPUT);
 
  // Set LED off
  digitalWrite(LED_BUILTIN, LOW);
 
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
 
  // Display progress on Serial monitor
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  // Print local IP address and start web server
  Serial.println("");
  Serial.print("WiFi connected at IP Address ");
  Serial.println(WiFi.localIP());
 
  // Start Server
  server.begin();
}

void loop() {
 
  WiFiClient client = server.available();   // Listen for incoming clients
 
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
 
            // Switch the LED on and off
            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("LED on");
              picoLEDState = "on";
              digitalWrite(LED_BUILTIN, HIGH);
            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("LED off");
              picoLEDState = "off";
              digitalWrite(LED_BUILTIN, LOW);
            }
 
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
 
            // Web Page Heading
            client.println("<body><h1>Pico W LED Control</h1>");
            client.print("Your IP: ");
            client.println(client.remoteIP());
 
            // Display current state, and ON/OFF buttons for Onboard LED
            client.println("<p>Onboard LED is " + picoLEDState + "</p>");
            
            // Set buttons
            if (picoLEDState == "off") {
              
              // CSS to style the on/off buttons On=Green Off = Grey
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #4CAF50;border: none; color: white; border-radius: 8px;padding: 16px 40px;transform: translateY(-6px);");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #808080;transform: translateY(-2px);}</style></head>");
              // Draw circle of radius r= to visualize LED status 
              client.println("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"25\" height=\"25\">\n");
              client.println("<circle cx=\"10\" cy=\"10\" r=\"8\" stroke=\"black\" stroke-width=\"2\" fill=\"grey\"/>\n");
              client.println("</g>\n</svg>\n");
              //picoLEDState is off, display the ON button
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>"); //added
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");

            } else {
 
              // CSS to style the on/off buttons On = Grey Off = Green
              client.println("<style>html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button {background-color: #808080;border: none; color: white; border-radius: 8px;padding: 16px 40px;transform: translateY(-6px);");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #F23A3A;transform: translateY(-2px);}</style></head>");
              // Draw circle of radius r= to visualize LED status 
              client.println("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"25\" height=\"25\">\n");
              client.println("<circle cx=\"10\" cy=\"10\" r=\"8\" stroke=\"black\" stroke-width=\"2\" fill=\"#66ff66\"/>\n");
              client.println("</g>\n</svg>\n");              
              //picoLEDState is on, display the OFF button
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");          //added #808080
            }

            client.println("</body></html>");
 
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
