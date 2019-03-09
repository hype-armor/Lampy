#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

#define LED 2
 
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize the output variables as outputs
  pinMode(LED,OUTPUT);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println(WiFi.localIP());
  server.begin();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /purewhite/on") >= 0) {
              Serial.println("purewhite/on");
              analogWrite(LED,102);

              client.stop();
            } else if (header.indexOf("GET /purewhite/off") >= 0) {
              Serial.println("purewhite/off");
              analogWrite(LED,205);

              client.stop();
            } else if (header.indexOf("GET /softwhite/on") >= 0) {
              Serial.println("softwhite/on");
              analogWrite(LED,410);

              client.stop();
            } else if (header.indexOf("GET /softwhite/off") >= 0) {
              Serial.println("softwhite/off");
              analogWrite(LED,1024);
              
              client.stop();
            } else if (header.indexOf("GET / ") >= 0) {
              // Display the HTML web page
              client.println("");
              client.println("");
              client.println("");
              client.println("<html>");
              client.println("    <head>");
              client.println("        <meta charset=\"utf-8\">");
              client.println("        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">");
              client.println("        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no\">");
              client.println("        <title>Lampy Lamp</title>");
              client.println("        <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
              client.println("        <style>");
              client.println("          button {background-color: #195B6A;border: none; color: white;padding: 16px 40px;text-decoration: none;font-size: 20px;margin: 2px;cursor: pointer;}");
              client.println("        </style>");
              client.println("    <script>");
              client.println("    function fire(x) {");
              client.println("        function processData(data) {");
              client.println("            // taking care of data");
              client.println("        }");
              client.println("        function handler() {");
              client.println("            if(this.status == 200 &&");
              client.println("            this.responseXML != null &&");
              client.println("            this.responseXML.getElementById('test').textContent) {");
              client.println("            processData(this.responseXML.getElementById('test').textContent);");
              client.println("            } else {");
              client.println("            }");
              client.println("        }");
              client.println("        var client = new XMLHttpRequest();");
              client.println("            client.onload = handler;");
              client.println("            client.open(\"GET\", x);");
              client.println("            client.send();");
              client.println("    };");
              client.println("    </script>");
              client.println("    </head>");
              client.println("    <body>");
              client.println("        <div class=\"container\">");
              client.println("            <div class=\"row\">");
              client.println("              <div class=\"col\"><button type=\"button\" class=\"btn btn-primary btn-block\" onclick=\"fire('purewhite/on');\">Pure White On</button></div>");
              client.println("              <div class=\"col\"><button type=\"button\" class=\"btn btn-primary btn-block\" onclick=\"fire('softwhite/on');\">Soft White On</button></div>");
              client.println("              <div class=\"w-100\"></div>");
              client.println("              <div class=\"col\"><button type=\"button\" class=\"btn btn-primary btn-block\" onclick=\"fire('purewhite/off');\">Pure White Off</button></div>");
              client.println("              <div class=\"col\"><button type=\"button\" class=\"btn btn-primary btn-block\" onclick=\"fire('softwhite/off');\">Soft White Off</button></div>");
              client.println("            </div>");
              client.println("          </div>");
              client.println("    </body>");
              client.println("</html>");
              client.println("");
              
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            }
          } else if (header.indexOf("GET /favicon.ico") >= 0) {
            break;
          }
          else { // if you got a newline, then clear currentLine
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
