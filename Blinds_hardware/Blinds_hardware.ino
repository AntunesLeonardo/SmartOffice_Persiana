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
unsigned int RSpin[1] = {4};

unsigned int encButton = 2;
int valTest[2] = {0, 100};
unsigned int verify = 0; // Verificação de estado

// EEPROM Values -----------------------------------------------
#define savesNumber 1    // Número de variáveis salvas
#define waitSeconds 10   // Tempo em stand-by até salvar

// Encoder pinout ----------------------------------------------
RotaryEncoder encoder(encoderA[0], encoderB[0]);

// Variáveis de tempo EEPROM ------------------------------------
int inicialTime = 0;     // Tempo inicial
int currentTime = 0;     // Tempo atual
// unsigned int verify = 0; // Verificação de estado
int serverRequest = 0;
int blindPosition = 0;

// Função leitura encoder ======================================
//void encodUpdate () {
//  static int pos = 0;
//  encoder.tick();
//  int newPos = encoder.getPosition();
//  
//  //Se a posicao foi alterada, mostra o valor no Serial Monitor
//  if (pos < newPos) {
//    pos = newPos;
//    X -= 1;
//    inicialTime = millis();
//    verify = 1;
//    Serial.println(X);
//  } else if (pos > newPos) {
//    pos = newPos;
//    X += 1;
//    inicialTime = millis();
//    verify = 1;
//    Serial.println(X);
//  }
//  
//  // Teste para salvar no EEPROM
//  currentTime = millis();
//  if (((inicialTime + waitSeconds * 1000) <= currentTime) && verify == 1) {
//    Serial.println("Salvo");
//    if (X != EEPROM.read(0)) {
//      Serial.print(X);
//      Serial.println(" Atualizado");
//      EEPROM.write(0, X);
//      EEPROM.commit();
//    }
//    verify = 0;
//  }
//}

void encoderUpdate() {
  static int pos = 0;
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
  encoderUpdate();
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

void reedSwitch(unsigned int i) {
  while(digitalRead(RSpin[0]) == LOW){
    blindUp(i);
  }
  blindStop(i);
  blindPosition = 0;
}

// SETUP --------------------------------------------------------------------
void setup() {
  // Pin mode definition
  for(int i=0; i<blindNumber; i++){
    pinMode(motorA[i], OUTPUT);
    pinMode(motorB[i], OUTPUT);
  }
  pinMode(encButton, INPUT);

  // Serial begin
  Serial.begin(115200);

  // EEPROM begin and read
  EEPROM.begin(savesNumber);
  serverRequest = EEPROM.read(0);
  Serial.println(serverRequest);

  reedSwitch(blindNumber);
}

// LOOP ---------------------------------------------------------------------
void loop() {
  blindControl(0, serverRequest);

  if(digitalRead(encButton) == LOW){
    verify = !verify;
    serverRequest = valTest[verify];
  }
}
