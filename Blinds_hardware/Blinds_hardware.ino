/**
 * @file Blinds_hardware.ino
 * 
 * @mainpage Hardware control for automated blinds
 * 
 * @section description Description
 * Sketch for reading encoder position, and controling motor according to position request.
 * 
 * @section circuit Circuit
 * - ESP32 DevKit V1.
 * - Rotary encoder module KY-040.
 * - Motor driver L298N.
 * - Reed Switch.
 * - 1K Resistor (Reed Switch pull down).
 * - 220 Resistor (Encoder button pull up).
 * 
 * @section libraries Libraries
 * - RotaryEncoder.h (https://github.com/mathertel/RotaryEncoder).
 * - EEPROM.h (https://github.com/PaulStoffregen/EEPROM).
 * 
 * @section author Author
 * - Created by Leonardo A Antunes
 * - https://github.com/AntunesLeonardo
 */

// Libraries ---------------------------------------------------
#include <WiFi.h>
#include <RotaryEncoder.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

// Defines -----------------------------------------------------
#define blindsNumber 1                                               ///< Number of blinds conected
#define rotationTime 1000                                           ///< Time for rotating 60º at 100 rpm

// Blinds ID ---------------------------------------------------
const char* blindID_0 = "001";
 
// Pinout constants --------------------------------------------
const unsigned int RSpin[blindsNumber] = {17};                       ///< Reed Switch input pin
const unsigned int encoderA[blindsNumber] = {5};                    ///< Encoder input port A pin
const unsigned int encoderB[blindsNumber] = {18};                    ///< Encoder input port B pin
const unsigned int vertMotorA[blindsNumber] = {19};                  ///< Vertical motor controler port A pin
const unsigned int vertMotorB[blindsNumber] = {21};                  ///< Vertical motor controler port B pin
const unsigned int rotMotorA[blindsNumber] = {22};                   ///< Rotation motor controler port A pin
const unsigned int rotMotorB[blindsNumber] = {23};                   ///< Rotation motor controler port B pin
const unsigned int LedAlert = 2;


// Variables ---------------------------------------------------
int serverVertRequest[blindsNumber] = {0};                           ///< Vertical position request from server - WIP
int serverRotRequest[blindsNumber] = {0};                            ///< Blades rotation request from server - WIP
int blindPosition[blindsNumber] = {0};                               ///< Current position of the blinds
int bladePosition[blindsNumber] = {0};                               ///< Corrent position of the blades

// Encoder pinout ----------------------------------------------
RotaryEncoder encoder(encoderA[0], encoderB[0]);                     //   Rotary encoder library pinout

// Internet - MQTT----------------------------------------------
const int mqttPort = 1883;                                           ///< MQTT standard port
const char* mqttServer = "179.106.217.205";                          ///< MQTT host
const char* mqttUser = "guest";                                      ///< MQTT User
const char* mqttPassword = "guest";                                  ///< MQTT Password
const char* topic = "/shutter";                                      ///< MQTT Topic
const char* ssid = "SmartPTI";                                       ///< Wifi ssid
const char* password = "SmartPT12017.";                              ///< Wifi password

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// -------------------------------------------------------------

/**
 * Control when the motor should turn on and off, and so it's direction.
 * 
 * @param blindID   Blind identification number.
 */
void blindControl(unsigned int blindID) {
  encoderUpdate(blindID);                                            //   blindPosition update
  Serial.print(blindPosition[blindID]);
  Serial.print("  ->  ");
  Serial.println(serverVertRequest[blindID]);
  
  if (serverVertRequest[blindID] == blindPosition[blindID]){         //   blindPosition achieved request
    blindStop(blindID);

    if (serverRotRequest[blindID] == 0) {                            // opens blades when requested
      rotateOpen(blindID);
    } else if (serverRotRequest[blindID] == 1) {                     // closes blades when requested
      rotateClose(blindID);
    }
    
    // Save last stable position value
    if ((blindPosition[blindID] != EEPROM.read(blindID)) || (bladePosition[blindID] != EEPROM.read(blindsNumber + blindID))) {
      Serial.print(blindPosition[blindID]);
      Serial.println(" Position Updated");
      EEPROM.write(blindID, blindPosition[blindID]);
      Serial.print(bladePosition[blindID]);
      Serial.println(" Blade position Updated");
      EEPROM.write(blindsNumber + blindID, bladePosition[blindID]);
      EEPROM.commit();
    }
  } else {
    rotateOpen(blindID);
    
    if (serverVertRequest[blindID] < blindPosition[blindID]) {       //   blindPosition is lower than request
      blindUp(blindID);

    } else if (serverVertRequest[blindID] > blindPosition[blindID]) {//   blindPosition is higher than request
      blindDown(blindID);
    }
  }
}

/**
 * At starting, define absolute zero value for blindPosition
 * 
 * @param i   Blind identification number
 */
void reedSwitch(unsigned int i) {
  Serial.print("reedSwitch i");
  Serial.println(i);
  Serial.println("Waiting for Reed Switch...");
  while(digitalRead(RSpin[i]) != HIGH){
    Serial.println(digitalRead(RSpin[i]));
    blindUp(i);
    delay(1000);
  }
  blindStop(i);
  blindPosition[i] = 0;
  Serial.print("Reed Switch comfirmed  ");
  Serial.println(blindPosition[i]);
}

// -------------------------------------------------------------

/**
 * Default setup function - pinMode definition and begining.
 */
void setup() {
  // Serial and EEPROM begin
  Serial.begin(115200);
  EEPROM.begin(2*blindsNumber);

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnect();
  client.subscribe(topic);

  for(int i=0; i<blindsNumber; i++){
    Serial.print("Setup For i");
    Serial.println(i);
    
    // Pin mode definition
    pinMode(vertMotorA[i], OUTPUT);
    pinMode(vertMotorB[i], OUTPUT);
    pinMode(rotMotorA[i], OUTPUT);
    pinMode(rotMotorB[i], OUTPUT);

    // EEPROM read
    serverVertRequest[i] = EEPROM.read(i);
    Serial.print("Last vertical position - blind ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(serverVertRequest[i]);

    serverRotRequest[i] = EEPROM.read(blindsNumber + i);
    Serial.print("Last rotational position - blind ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(serverRotRequest[i]);

    // Reed Switch starting
    reedSwitch(i);
  }
  pinMode(LedAlert, OUTPUT);
}

// -------------------------------------------------------------

/**
 * Default loop funtion.
 */
void loop() {
  client.loop();
  for(int i=0; i<blindsNumber; i++){
    while((serverVertRequest[i] != blindPosition[i])||(serverRotRequest[i] != bladePosition[i])){
      blindControl(i);
      delay(1);
    }
  }
  delay(10);
}
