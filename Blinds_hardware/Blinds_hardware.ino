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
#include <RotaryEncoder.h>
#include <EEPROM.h>

// Defines -----------------------------------------------------
#define blindsNumber 1                                               ///< Number of blinds conected

// Pinout constants --------------------------------------------
const unsigned int encoderA[blindsNumber] = {32};                    ///< Encoder input port A pin
const unsigned int encoderB[blindsNumber] = {33};                    ///< Encoder input port B pin
const unsigned int motorA[blindsNumber] = {18};                      ///< Motor controler write port A pin
const unsigned int motorB[blindsNumber] = {19};                      ///< Motor controler write port B pin
const unsigned int RSpin[blindsNumber] = {4};                        ///< Reed Switch input pin

const unsigned int encButton = 2;                                    ///< Encoder button pin (testing)

// Variables ---------------------------------------------------
int serverRequest[blindsNumber] = {0};                               ///< Position request from server - WIP
int blindPosition[blindsNumber] = {0};                               ///< Current position of the blinds

int valTest[2] = {0, 100};                                           ///< Fake request values (testing)
unsigned int verify = 0;                                             ///< Status verification (testing)

// Encoder pinout ----------------------------------------------
RotaryEncoder encoder(encoderA[0], encoderB[0]);                     //   Rotary encoder library pinout

/**
 * Reads encoder values and updates blindPosition variable
 * 
 * @param i   Blind identification number.
 */
void encoderUpdate(unsigned int i) {
  static int pos = 0;  
  int newPos = 0;                                                    ///< Auxiliar value for geting rotation direction
  switch (i){
    case 1:
      encoder.tick();
      newPos = encoder.getPosition();                                ///< Auxiliar value for reading position
      break;
  }

  if (pos < newPos) {
    pos = newPos;
    blindPosition[i-1]--;
    Serial.println(blindPosition[i-1]);
  } else if (pos > newPos) {
    pos = newPos;
    blindPosition[i-1]++;
    Serial.println(blindPosition[i-1]);
  }
}

/**
 * Turn the motor on, so the blinds go down.
 * 
 * @param i   Motor identification number.
 */ 
void blindDown(unsigned int i) {
  Serial.print("Motor ");
  Serial.print(i);
  Serial.println(" - Go Down");
  digitalWrite(motorA[i-1], HIGH);
  digitalWrite(motorB[i-1], LOW);
}

/**
 * Turn the motor on, so the blinds go up.
 * 
 * @param i   Motor identification number.
 */ 
void blindUp(unsigned int i) {
  Serial.print("Motor ");
  Serial.print(i);
  Serial.println(" - Go Up");
  digitalWrite(motorA[i-1], LOW);
  digitalWrite(motorB[i-1], HIGH);
}

/**
 * Turn the motor off.
 * 
 * @param i   Motor identification number.
 */ 
void blindStop(unsigned int i) {
  Serial.print("Motor ");
  Serial.print(i);
  Serial.println(" - Stop");
  digitalWrite(motorA[i-1], LOW);
  digitalWrite(motorB[i-1], LOW);
}

/**
 * Control when the motor should turn on and off, and so it's direction.
 * 
 * @param blindID   Blind identification number.
 */
void blindControl(unsigned int blindID) {
  encoderUpdate(blindID);                                            //   blindPosition update
  Serial.print(blindPosition[blindID-1]);
  Serial.print("  ->  ");
  Serial.println(serverRequest[blindID-1]);
  if (serverRequest[blindID-1] == blindPosition[blindID-1]){         //   blindPosition achieved request
    blindStop(blindID);
    
    // Save last stable position value
    if (blindPosition[blindID-1] != EEPROM.read(blindID-1)) {
      Serial.print(blindPosition[blindID-1]);
      Serial.println(" Atualizado");
      EEPROM.write(blindID-1, blindPosition[blindID-1]);
      EEPROM.commit();
  }
  } else if (serverRequest[blindID-1] > blindPosition[blindID-1]) {  //   blindPosition is lower than request
    blindUp(blindID);

  } else if (serverRequest[blindID-1] < blindPosition[blindID-1]) {  //   blindPosition is higher than request
    blindDown(blindID);
  }
}

/**
 * At starting, define absolute zero value for blindPosition
 * 
 * @param i   Blind identification number
 */
void reedSwitch(unsigned int i) {
  Serial.println("Waiting for Reed Switch...");
  while(digitalRead(RSpin[i-1]) != HIGH){
    Serial.println(digitalRead(RSpin[i-1]));
    blindUp(i);
    delay(1);
  }
  blindStop(i);
  blindPosition[i-1] = 0;
  Serial.print("Reed Switch comfirmed  ");
  Serial.println(blindPosition[i-1]);
}

/**
 * Default setup function - pinMode definition and begining.
 */
void setup() {
  // Serial and EEPROM begin
  Serial.begin(115200);
  EEPROM.begin(blindsNumber);

  for(int i=0; i<blindsNumber; i++){
    // Pin mode definition
    pinMode(motorA[i], OUTPUT);
    pinMode(motorB[i], OUTPUT);

    // EEPROM read
    serverRequest[i] = EEPROM.read(i);
    Serial.print("Last request at ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.println(serverRequest[i]);

    // Reed Switch starting
    reedSwitch(i+1);
  }
  pinMode(encButton, INPUT);
}

/**
 * Default loop funtion.
 */
void loop() {
  for(int i=1; i<=blindsNumber; i++){
    blindControl(i);
  }
  
  // Test ambient for simulating requests
  if(digitalRead(encButton) == LOW){
    verify = !verify;
    serverRequest[0] = valTest[verify];
  }
  delay(1);
}
