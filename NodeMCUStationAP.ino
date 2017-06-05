/*....................................................
* Author: Sanyam Arya
* ersanyamarya@gmail.com
* https://www.facebook.com/er.sanyam.arya
* https://www.linkedin.com/in/sanyam-arya-077ab638/
......................................................*/
//Including WiFi library
#include <ESP8266WiFi.h>
//Incluiding web Server Library
#include <ESP8266WebServer.h>
//Including DNS server IP
#include <ESP8266mDNS.h>

int flag = 0; // flag to control blinking

MDNSResponder mdns;      //Dns Responder variable
ESP8266WebServer server; //Web server variable

uint8_t pin_led = 16; //Utility LED PIN
uint8_t pin_con = 16; //Status LED PIN

//Acces Point to which you want to conncet
char *ssid = "iotServer";
char *password = "qwerty123456";

//Node-MCU as a AP
char *mySsid = "SanyamArya";
char *myPassword = "password";
//Settings for the AP
IPAddress ip(192, 168, 11, 4);
IPAddress gateway(192, 168, 11, 1);
IPAddress subnet(255, 255, 255, 0);
//---------------------------------------------------------------------------------------------------
void setup()
{
  //Setting modes for the pins
  pinMode(pin_led, OUTPUT);
  pinMode(pin_con, OUTPUT);

  WiFi.mode(WIFI_AP_STA); //Setting the Wifi status

  WiFi.begin(ssid, password); //Begaining the connection to the AP
  Serial.begin(115200);       // Begaining the serial interface

  digitalWrite(pin_con, LOW); // Initialising digital pin

  /*Setting up the wifi connection to the AP and checking connection
 *Blinking the LED if it is connecting (pin_con)
 *LED Stops blinking once connceted  (pin_con)
 */
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(pin_con, !digitalRead(pin_con));
    delay(250);
    digitalWrite(pin_con, !digitalRead(pin_con));
    delay(250);
  }
  digitalWrite(pin_con, LOW);

  //Printing the IP Address Asigned to the Node-MCU
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // setting up DNS name for the IP assigned
  if (mdns.begin("sanyamarya", WiFi.localIP()))
  {
    Serial.println("Responder started for local IP : sanyamarya.local");
  }

  //setting up the server data
  String str = String("Hello World.. !\n1. toggle\n2. blink\n3. stop\n4. adc");
  //server.on("/", []() { server.send(200, "text/plain", "Hello World!"); });
  server.on("/", []() {
    server.send(200, "text/plain", "Hello World.. !/n1. toggle/n2. blink/n3. stop/n4. adc");
    });
  server.on("/toggle", toggleLED); // server.on("/<ip/toggle>",<function>);
  server.on("/blink", blinkLED);
  server.on("/stop", stopLED);
  server.on("/adc", adcRead);
  server.begin(); //begaining the HTTPS server

  //configuring the NODE-MCU as a AP
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(mySsid, myPassword);
  //adding services to DNS services
  MDNS.addService("http", "tcp", 80);
}

void loop()
{
  server.handleClient(); //default method

  if (flag == 1) // if 1 blink LED
  {
    digitalWrite(pin_led, !digitalRead(pin_led));
    delay(300);
    digitalWrite(pin_led, !digitalRead(pin_led));
    delay(300);
  }
}

//All the functions are defined here
void blinkLED()
{
  flag = 1;
  server.send(200, "text/plain", "LED Blinking!");
}
void stopLED()
{
  flag = 0;
  digitalWrite(pin_led, LOW);
  server.send(200, "text/plain", "LED Stopped!");
}
void toggleLED()
{
  flag = 0;
  digitalWrite(pin_led, !digitalRead(pin_led));
  server.send(200, "text/plain", "LED Toggled!");
}
void adcRead()
{
  int sens = analogRead(A0);
  String one1 = String(sens);
  String one = String("ADC: "+one1);
  server.send(200, "text/plain", one);
}
