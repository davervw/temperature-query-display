// Modifications for remote temperature server
// for ESP8266/OLED/18650 WeMos (D1) device
// Copyright (c) 2022 by David Van Wagner
// davevw.com
// Open source: MIT License

// Based on ESP8266 Web Server example

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "SSD1306.h"
//#include "ssid_password.h"

#ifndef STASSID
#define STASSID "SSID"
#define STAPSK  "password1234!@#$"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);

const int led = LED_BUILTIN;

const String postForms = "<html>\
  <head>\
    <title>Temperature Sensor</title>\
  </head>\
  <body>\
    <a href=\"temp\">Temperature</a>\
  </body>\
";
//     <button>Toggle</button>\

//const String postForms = "<html>\
//  <head>\
//    <title>ESP8266 Web Server POST handling</title>\
//    <style>\
//      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
//    </style>\
//  </head>\
//  <body>\
//    <h1>POST plain text to /postplain/</h1><br>\
//    <form method=\"post\" enctype=\"text/plain\" action=\"/postplain/\">\
//      <input type=\"text\" name=\"hello\" value='{ \"hello\": \"world\", \"trash\": \"can\" }'\"><br>\
//      <input type=\"submit\" value=\"Submit\">\
//    </form>\
//    <h1>POST form data to /postform/</h1><br>\
//    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
//      <input type=\"text\" name=\"hello\" value=\"world\"><br>\
//      <input type=\"submit\" value=\"Submit\">\
//    </form>\
//  </body>\
//</html>";

void handleRoot() {
  //digitalWrite(led, 0);
  server.send(200, "text/html", postForms);
  //digitalWrite(led, 1);
}

int raw;
int fahrenheit;

void handleTemp() 
{  
  if (server.method() == HTTP_GET) {
    digitalWrite(led, 0);
    static char message[80];
    sprintf(message, "{ \"raw\": %d, \"fahrenheit\": %d }", raw, fahrenheit);
    server.send(200, "application/json", message);
    delay(200);
    digitalWrite(led, 1);
  } else {
    //digitalWrite(led, 0);
    server.send(405, "text/plain", "Method Not Allowed");
    //digitalWrite(led, 1);
  }
}

void handleToggle() {
  static int toggle = 0;
  if (server.method() == HTTP_GET) {
    toggle = 1-toggle;
    digitalWrite(led, toggle);
    server.send(200, "text/plain", "GET request was /toggle\n");
    display.clear();
    if (toggle)
    {
      display.setFont(ArialMT_Plain_24);
      display.drawString(0, 26, "Blink");
      display.setFont(ArialMT_Plain_10);
    }
    display.display();
  } else {
    //digitalWrite(led, 0);
    server.send(405, "text/plain", "Method Not Allowed");
    //digitalWrite(led, 1);
  }
}

//void handlePlain() {
//  if (server.method() == HTTP_POST) {
//    digitalWrite(led, 0);
//    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
//    delay(200);
//    digitalWrite(led, 1);
//  } else {
//    //digitalWrite(led, 0);
//    server.send(405, "text/plain", "Method Not Allowed");
//    //digitalWrite(led, 1);
//  }
//}
//
//void handleForm() {
//  if (server.method() == HTTP_POST) {
//    //digitalWrite(led, 0);
//    String message = "POST form was:\n";
//    for (uint8_t i = 0; i < server.args(); i++) {
//      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//    }
//    server.send(200, "text/plain", message);
//    //digitalWrite(led, 1);
//  } else {
//    //digitalWrite(led, 0);
//    server.send(405, "text/plain", "Method Not Allowed");
//    //digitalWrite(led, 1);
//  }
//}

void handleNotFound()
{
  //digitalWrite(led, 0);
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
  //digitalWrite(led, 1);
}

void setup(void) {
  // Initialising the UI will init the display too.
  display.init();

  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(ArialMT_Plain_10);
//  display.setFont(ArialMT_Plain_16);
//  display.setFont(ArialMT_Plain_24);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
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

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 26, WiFi.localIP().toString());
  display.display();
  display.setFont(ArialMT_Plain_10);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.enableCORS(true);

  server.on("/", handleRoot);

  server.on("/temp", handleTemp);

  server.on("/toggle", handleToggle);

//  server.on("/postplain/", handlePlain);
//
//  server.on("/postform/", handleForm);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void drawChar(char c)
{
  static int row=0;
  static int col=0;

  int x=col*12;
  if (x+12 > 128)
  {
    ++row;
    x = col = 0;
  }

  int y=row*10;
  if (y+10 > 64)
  {
    y = row = 0;
    display.clear();
  }

  if (c >= ' ' && c <= '~')
  {
    char s[2];
    s[0] = c;
    s[1] = 0;
    display.setFont(ArialMT_Plain_10);
    display.drawString(x, y, s);
    display.display();
    ++col;
  }
  else if (c == 10)
    ++row;
  else if (c == 13)
    col=0;
  else if (c == 12)
  {
    col = row = 0;
    display.clear();
  }
}

void clearFirst()
{
  static bool cleared = false;
  if (cleared || Serial.available() <= 0)
    return;
  display.clear();
  cleared = true;
}

void displayTemp()
{
  static char temp[20];
  sprintf(temp, "%d", fahrenheit);
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 0, temp);
  display.display();
}

void handleSerial() 
{
  static char line[80];
  static int i = 0;
  
  //clearFirst();

  if (Serial.available() > 0)
  {
    char c = Serial.read();
    //drawChar(c);
    bool unprintable = (c < ' ' || c > '~');
    if (c == '\n')
    {
      if (strncmp(line, "raw=", 4)== 0)
        raw = atoi(&line[4]);
      else if (strncmp(line, "fahrenheit=", 11)==0)
      {
        fahrenheit = atoi(&line[11]);
        displayTemp();
      }
      i = 0;
    }
    if (unprintable)
      return;
    if (i<79)
    {
      line[i] = c;
      line[++i] = 0;    
    }
  }
}

void loop(void)
{
  server.handleClient();
  handleSerial();
}

