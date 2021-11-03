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
#define rotationTime 0.278                                           ///< Time for rotating 60º at 100 rpm

// Pinout constants --------------------------------------------
const unsigned int RSpin[blindsNumber] = {25};                       ///< Reed Switch input pin
const unsigned int encoderA[blindsNumber] = {32};                    ///< Encoder input port A pin
const unsigned int encoderB[blindsNumber] = {33};                    ///< Encoder input port B pin
const unsigned int vertMotorA[blindsNumber] = {34};                  ///< Vertical motor controler port A pin
const unsigned int vertMotorB[blindsNumber] = {35};                  ///< Vertical motor controler port B pin
const unsigned int rotMotorA[blindsNumber] = {36};                   ///< Rotation motor controler port A pin
const unsigned int rotMotorB[blindsNumber] = {39};                   ///< Rotation motor controler port B pin

const unsigned int encButton = 2;                                    ///< Encoder button pin (testing)

// Variables ---------------------------------------------------
int serverVertRequest[blindsNumber] = {0};                           ///< Vertical position request from server - WIP
int serverRotRequest[blindsNumber] = {0};                            ///< Blades rotation request from server - WIP
int blindPosition[blindsNumber] = {0};                               ///< Current position of the blinds
int bladePosition[blindsNumber] = {0};                               ///< Corrent position of the blades

int valTest[2] = {0, 50};                                            ///< Fake request values (testing)
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
  digitalWrite(vertMotorA[i-1], HIGH);
  digitalWrite(vertMotorB[i-1], LOW);
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
  digitalWrite(vertMotorA[i-1], LOW);
  digitalWrite(vertMotorB[i-1], HIGH);
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
  digitalWrite(vertMotorA[i-1], LOW);
  digitalWrite(vertMotorB[i-1], LOW);
}

/**
 * Rotates blades to open position
 * 
 * @param i Motor identification number
 */
void rotateOpen(unsigned int i) {
  if (bladePosition[i-1] != 0) {
    Serial.print("Motor ");
    Serial.print(i);
    Serial.println(" - Open");
    digitalWrite(rotMotorA[i-1], LOW);
    digitalWrite(rotMotorA[i-1], HIGH);
    delay(rotationTime);
    digitalWrite(rotMotorA[i-1], LOW);
    digitalWrite(rotMotorA[i-1], LOW);
    bladePosition[i-1] = 0;
  }
}

/**
 * Rotate blades to closed position
 * 
 * @param i Motor identification number
 */
void rotateClose(unsigned int i) {
  if (bladePosition[i-1] == 0) {
    Serial.print("Motor ");
    Serial.print(i);
    Serial.println(" - Close");
    digitalWrite(rotMotorA[i-1], HIGH);
    digitalWrite(rotMotorA[i-1], LOW);
    delay(rotationTime);
    digitalWrite(rotMotorA[i-1], LOW);
    digitalWrite(rotMotorA[i-1], LOW);
    bladePosition[i-1] = 1;
  }
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
  Serial.println(serverVertRequest[blindID-1]);
  if (serverVertRequest[blindID-1] == blindPosition[blindID-1]){     //   blindPosition achieved request
    blindStop(blindID);
    
    // Save last stable position value
    if (blindPosition[blindID-1] != EEPROM.read(blindID-1)) {
      Serial.print(blindPosition[blindID-1]);
      Serial.println(" Position Updated");
      EEPROM.write(blindID-1, blindPosition[blindID-1]);
      EEPROM.commit();
  }
  } else if (serverVertRequest[blindID-1] > blindPosition[blindID-1]) {  //   blindPosition is lower than request
    blindUp(blindID);

  } else if (serverVertRequest[blindID-1] < blindPosition[blindID-1]) {  //   blindPosition is higher than request
    blindDown(blindID);
  }

  if (serverRotRequest[blindID-1] == 0) {                          // opens blades when requested
    rotateOpen(blindID);
  } else if (serverRotRequest[blindID-1] == 1) {                   // closes blades when requested
    rotateClose(blindID);
  }

  if (bladePosition[blindID-1] != EEPROM.read(blindsNumber + blindID-1)) {
    Serial.print(bladePosition[blindID-1]);
    Serial.println(" Blade position Updated");
    EEPROM.write(blindsNumber + blindID-1, bladePosition[blindID-1]);
    EEPROM.commit();
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
  EEPROM.begin(2*blindsNumber);

  for(int i=0; i<blindsNumber; i++){
    // Pin mode definition
    pinMode(vertMotorA[i], OUTPUT);
    pinMode(vertMotorB[i], OUTPUT);
    pinMode(rotMotorA[i], OUTPUT);
    pinMode(rotMotorB[i], OUTPUT);

    // EEPROM read
    serverVertRequest[i] = EEPROM.read(i);
    Serial.print("Last vertical position - blind ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.println(serverVertRequest[i]);

    serverRotRequest[i] = EEPROM.read(blindsNumber + i);
    Serial.print("Last rotational position - blind ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.println(serverRotRequest[i]);

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
    serverVertRequest[0] = valTest[verify];
  }
  delay(1);
}
