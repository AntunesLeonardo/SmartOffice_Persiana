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

// Pinout constants --------------------------------------------
const unsigned int encoderA = 32;                ///< Encoder input port A pin
const unsigned int encoderB = 33;                ///< Encoder input port B pin
const unsigned int motorA = 18;                  ///< Motor controler write port A pin
const unsigned int motorB = 19;                  ///< Motor controler write port B pin
const unsigned int RSpin = 4;                    ///< Reed Switch input pin

const unsigned int encButton = 2;                ///< Encoder button pin (testing)

// Variables ---------------------------------------------------
int serverRequest = 0;                           ///< Position request from server - WIP
int blindPosition = 0;                           ///< Current position of the blinds

int valTest[2] = {0, 100};                       ///< Fake request values (testing)
unsigned int verify = 0;                         ///< Status verification (testing)

// Defines -----------------------------------------------------
#define blindsNumber 1                           ///< Number of blinds conected

// Encoder pinout ----------------------------------------------
RotaryEncoder encoder(encoderA, encoderB);       //   Rotary encoder library pinout

/**
 * Reads encoder values and updates blindPosition variable
 */
void encoderUpdate() {
  static int pos = 0;                            ///< Auxiliar value, indicates rotation direction
  encoder.tick();
  int newPos = encoder.getPosition();

  if (pos < newPos) {
    pos = newPos;
    blindPosition--;
    Serial.println(blindPosition);
  } else if (pos > newPos) {
    pos = newPos;
    blindPosition++;
    Serial.println(blindPosition);
  }
}

/**
 * Turn the motor on, so the blinds go down.
 * 
 * @param i   Motor identification number.
 */ 
void blindDown(unsigned int i) {
  Serial.println("Motor - Go Down");
  digitalWrite(motorA, HIGH);
  digitalWrite(motorB, LOW);
}

/**
 * Turn the motor on, so the blinds go up.
 * 
 * @param i   Motor identification number.
 */ 
void blindUp(unsigned int i) {
  Serial.println("Motor - Go Up");
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, HIGH);
}

/**
 * Turn the motor off.
 * 
 * @param i   Motor identification number.
 */ 
void blindStop(unsigned int i) {
  Serial.println("Motor - Stop");
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);
}

/**
 * Control when the motor should turn on and off, and so it's direction.
 * 
 * @param blindID   Blind identification number.
 * @param request   Request for new position.
 */
void blindControl(unsigned int blindID, int request) {
  encoderUpdate();                               //   blindPosition update
  Serial.print(blindPosition);
  Serial.print("  ->  ");
  Serial.println(request);
  if (request == blindPosition){                 //   blindPosition achieved request
    blindStop(blindID);
    
    // Save last stable position value
    if (blindPosition != EEPROM.read(blindID)) {
      Serial.print(blindPosition);
      Serial.println(" Atualizado");
      EEPROM.write(blindID, blindPosition);
      EEPROM.commit();
  }
  } else if (request > blindPosition) {          //   blindPosition is lower than request
    blindUp(blindID);

  } else if (request < blindPosition) {          //   blindPosition is higher than request
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
  while(digitalRead(RSpin) != HIGH){
    Serial.println(digitalRead(RSpin));
    blindUp(i);
    delay(1);
  }
  blindStop(i);
  blindPosition = 0;
  Serial.print("Reed Switch comfirmed  ");
  Serial.println(blindPosition);
}

/**
 * Default setup function - pinMode definition and begining.
 */
void setup() {
  // Pin mode definition
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(encButton, INPUT);

  // Serial begin
  Serial.begin(115200);

  // EEPROM begin and read
  EEPROM.begin(blindsNumber);
  serverRequest = EEPROM.read(0);
  Serial.println(serverRequest);

  // Reed Switch starting
  reedSwitch(blindsNumber);
}

/**
 * Default loop funtion.
 */
void loop() {
  blindControl(0, serverRequest);

  // Test ambient for simulating requests
  if(digitalRead(encButton) == LOW){
    verify = !verify;
    serverRequest = valTest[verify];
  }
  delay(1);
}
