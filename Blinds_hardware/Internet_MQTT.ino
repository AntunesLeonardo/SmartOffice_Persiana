/**
 * @file Internet_MQTT.ino
 * 
 * @mainpage Hardware control for automated blinds
 * 
 * @section description Description
 * Functions related to connection and communication through MQTT.
 */

 void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin((char*)ssid, password);
  //showConnectionWarningStatus();
  delay(200);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //showWhiteWarningStatus();
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //showOkStatus();
}

void reconnect() {
  while (WiFi.status() != WL_CONNECTED) {
    showWhiteWarningStatus();
    delay(500);
  }
  while (!client.connected()) {
    //showWhiteWarningStatus();
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      client.publish(TOPICO, "Reconectado");
    } else {
      int estado = client.state();
      String t = horario_atual() + String("Falhou, estado do cliente = " + estado);
      publishSerialData(TOPICO,(char*)t.c_str());
      delay(5000);
    }
  }
  //showOkStatus();
}

void callback(char* topic, byte *payload, unsigned int len) {

}
