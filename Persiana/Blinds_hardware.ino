// Automatic blinds control for offices - hardware.
// Board: ESP32.
// Author: Leonardo A. Antunes - PTI
 
// Library -----------------------------------------------------
#include <RotaryEncoder.h>
#include <EEPROM.h>

// Pinout definition -------------------------------------------
#define blindNumber 1
unsigned int encoderA[1] = {32};                 // Encoder analog read port A
unsigned int encoderB[1] = {33};                 // Encoder analog read port B
unsigned int motorA[1] = {18};                   // Motor controler write port A
unsigned int motorB[1] = {19};                   // Motor controler write port B

// EEPROM Values -----------------------------------------------
#define savesNumber 1    // Número de variáveis salvas
#define waitSeconds 10   // Tempo em stand-by até salvar

// Encoder pinout ----------------------------------------------
RotaryEncoder encoder(encoderA[0], encoderB[0]);

// Variáveis de tempo EEPROM ------------------------------------
int inicialTime = 0;     // Tempo inicial
int currentTime = 0;     // Tempo atual
unsigned int verify = 0; // Verificação de estado
int serverRequest = 0;

int encoderUpdate() {
  encoder.tick();
  int newPos = encoder.getPosition();
  return newPos;
}

// Funções individuais - Desce - Sobe - Para --------------------------------
/*
 * Controle do motor pelas porta da ponte H.
 */
void blindDown(unsigned int i) {
  digitalWrite(motorA[i], HIGH);
  digitalWrite(motorB[i], LOW);
}

void blindUp(unsigned int i) {
  digitalWrite(motorA[i], LOW);
  digitalWrite(motorB[i], HIGH);
}

void blindStop(unsigned int i) {
  digitalWrite(motorA[i], LOW);
  digitalWrite(motorB[i], LOW);
}

// Controle sobre o acionamento do motor ------------------------------------
/*
 * Identifica pedidos de movimentação e coordena a movimentação.
 */
void blindControl(unsigned int blindID, int request) {
  int blindPosition = encoderUpdate();
  if (request == blindPosition){
    blindStop(blindID);
    
    if (blindPosition != EEPROM.read(blindID)) {
      Serial.print(blindPosition);
      Serial.println(" Atualizado");
      EEPROM.write(blindID, blindPosition);
      EEPROM.commit();
  }
  } else if (request > blindPosition) {
    blindUp(blindID);
  } else if (request < blindPosition) {
    blindDown(blindID);
  }
}

// SETUP --------------------------------------------------------------------
void setup() {
  // Pin mode definition
  for(int i=0; i<blindNumber; i++){
    pinMode(motorA[i], OUTPUT);
    pinMode(motorB[i], OUTPUT);
  }

  // Serial begin
  Serial.begin(115200);

  // EEPROM begin and read
  EEPROM.begin(savesNumber);
  serverRequest = EEPROM.read(0);
  Serial.println(serverRequest);
}

// LOOP ---------------------------------------------------------------------
void loop() {
  blindControl(0, serverRequest);
}
