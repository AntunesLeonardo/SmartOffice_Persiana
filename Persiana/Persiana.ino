#include <RotaryEncoder.h>
#include <EEPROM.h>

#define Bencod 2
#define pHa 18
#define pHb 19

#define savesNumber 1
#define waitSeconds 10

// Pinos de ligacao do encoder
RotaryEncoder encoder(32, 33);

// Variaveis de posicionamento
int newPos = 0;
int X = 0;
int request = 0;

// TESTE:
unsigned int i = 0;
int testRequest[5] = {50, 100, 70, 30, 0};

// Variáveis de tempo EEPROM
int inicialTime = 0;
int currentTime = 0;
unsigned int verify = 0;

// Função leitura encoder ---------------------------------------------------
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

// Controle sobre o acionamento do motor-------------------------------------
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
  pinMode(Bencod, INPUT);
  pinMode(pHa, OUTPUT);
  pinMode(pHb, OUTPUT);
  Serial.begin(115200);

  EEPROM.begin(savesNumber);
  X = EEPROM.read(0);
  Serial.println(X);
  
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
