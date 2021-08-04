/* 
 * Programa para controle de persiana em ambiente de escritório.
 * Placa ESP32.
 * Autor: Leonardo A. Antunes - PTI
 */
// Inclusão de bibliotecas --------------------------------------
#include <RotaryEncoder.h>
#include <EEPROM.h>

// Definição de portas ------------------------------------------
#define Bencod 2         // Botão encoder
#define encoder1 32      // Porta 1 leitura encoder
#define encoder2 33      // Porta 2 leitura encoder
#define pHa 18           // Ponte H, porta A
#define pHb 19           // Ponte H, porta B

// Valores EEPROM -----------------------------------------------
#define savesNumber 1    // Número de variáveis salvas
#define waitSeconds 10   // Tempo em stand-by até salvar

// Pinos de ligacao do encoder ----------------------------------
RotaryEncoder encoder(encoder1, encoder2);

// Variaveis de posicionamento ----------------------------------
int newPos = 0;          // Posição nova - identifica movimento
int X = 0;               // Posição global
int request = 0;         // Pedido para movimentar

// TESTE: --------------------------------------------(temporário)
unsigned int i = 0;
int testRequest[5] = {50, 100, 70, 30, 0};

// Variáveis de tempo EEPROM ------------------------------------
int inicialTime = 0;     // Tempo inicial
int currentTime = 0;     // Tempo atual
unsigned int verify = 0; // Verificação de estado

// Função leitura encoder ======================================
/*
 * Lê nava posição do encoder e atualiza a variável.
 * X atualizado e print serial da posição global (X).
 * Caso a posição se mantenha estática por um período, a última
 * posição é salva no EEPROM.
 */
void encodUpdate () {
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition();
  
  //Se a posicao foi alterada, mostra o valor no Serial Monitor
  if (pos < newPos) {
    pos = newPos;
    X -= 1;
    inicialTime = millis();
    verify = 1;
    Serial.println(X);
  } else if (pos > newPos) {
    pos = newPos;
    X += 1;
    inicialTime = millis();
    verify = 1;
    Serial.println(X);
  }
  
  // Teste para salvar no EEPROM
  currentTime = millis();
  if (((inicialTime + waitSeconds * 1000) <= currentTime) && verify == 1) {
    Serial.println("Salvo");
    if (X != EEPROM.read(0)) {
      Serial.print(X);
      Serial.println(" Atualizado");
      EEPROM.write(0, X);
      EEPROM.commit();
    }
    verify = 0;
  }
}

// Funções individuais - Desce - Sobe - Para --------------------------------
/*
 * Controle do motor pelas porta da ponte H.
 */
void wDown () {
  digitalWrite(pHa, HIGH);
  digitalWrite(pHb, LOW);
}
void wUp () {
  digitalWrite(pHa, LOW);
  digitalWrite(pHb, HIGH);
}
void wStop () {
  digitalWrite(pHa, LOW);
  digitalWrite(pHb, LOW);

  //delay(1000); // Teste ----
  //request += 50;
  Serial.println(request);
}

// Controle sobre o acionamento do motor ------------------------------------
/*
 * Identifica pedidos de movimentação e coordena a movimentação.
 */
void windowControl () {
  if (request == X){
    wStop();
    i += 1;
    if (i >= 5) i = 0;
    request = testRequest[i];
  } else if (request > X) {
    wUp();
  } else if (request < X) {
    wDown();
  }
}

// SETUP --------------------------------------------------------------------
void setup() {
  // Definição de estado das portas
  pinMode(Bencod, INPUT);
  pinMode(pHa, OUTPUT);
  pinMode(pHb, OUTPUT);

  // Inicío serial
  Serial.begin(115200);

  // Início EEPROM - leitura do valor salvo
  EEPROM.begin(savesNumber);
  request = EEPROM.read(0);
  Serial.println(request);
  
  Serial.println("Gire o encoder....");
  Serial.println(request);

}

// LOOP ---------------------------------------------------------------------
void loop() {
  //Verifica se o botao do encoder foi pressionado
  if (digitalRead(Bencod) != HIGH) {
    Serial.println("Botao pressionado");
    while (digitalRead(Bencod) == 0)
      delay(10);
  }

  encodUpdate();
  windowControl();

}
