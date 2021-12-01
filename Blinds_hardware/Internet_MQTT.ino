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
    Serial.print(".");
    //showWhiteWarningStatus();
    delay(100);
  }
  while (!client.connected()) {
    //showWhiteWarningStatus();
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      client.publish(topic, "Reconectado");
      Serial.println("Publish");
    } else {
      int estado = client.state();
      //String t = horario_atual() + String("Falhou, estado do cliente = " + estado);
      Serial.println("Not Conected");
      delay(5000);
    }
  }
  //showOkStatus();
}

void callback(char* topic, byte *payload, unsigned int len) {

}
