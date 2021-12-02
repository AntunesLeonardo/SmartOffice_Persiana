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
    digitalWrite(LedAlert, HIGH);
    delay(50);
    digitalWrite(LedAlert, LOW);
    delay(50);
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

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument <256> doc;
  deserializeJson(doc,payload);
  
  serverVertRequest[0] = doc[blindID_0][0];
  serverRotRequest[0] = doc[blindID_0][1];
  Serial.println(serverVertRequest[0]);
  Serial.println(serverRotRequest[0]);
}
