#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "UPDATESSID"
#define STAPSK  "UPDATEPASSWD"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

const String postForms = "<html>\
  <head>\
    <title>Viasat Relay Controller</title>\
    <link href=\"https://unpkg.com/material-components-web@latest/dist/material-components-web.min.css\" rel=\"stylesheet\">\
    <script src=\"https://unpkg.com/material-components-web@latest/dist/material-components-web.min.js\"></script>\
    <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\">\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      .mdc-button { width:100%; height:100%; font-size:xx-large; }\
      .container { width:50%; height:100%; align-items:center; display:flex; padding:1%; }\
    </style>\  
  </head>\
  <body>\
    <div class=\"flex-container\" style=\"display:flex; flex-direction: column; justify-content: center; align-items: center; height: 100%;\">\
      <div class=\"container\"><button class=\"mdc-button mdc-button--raised\" onclick=\"window.location.href = \'/on\';\">Turn On</button></div>\
      <div class=\"container\"><button class=\"mdc-button mdc-button--raised\" onclick=\"window.location.href = \'/off\';\">Turn Off</button></div>\
    </div>\
  </body>\
</html>";

void handleRoot() {
  server.send(200, "text/html", postForms);
}

void handleOn() {
  digitalWrite(2, 1);
  server.send(200, "text/html", postForms);
  Serial.println("Turning IO on");
}

void handleOff() {
  digitalWrite(2, 0);
  server.send(200, "text/html", postForms);
  Serial.println("Turning IO off");
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/on", handleOn);

  server.on("/off", handleOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
