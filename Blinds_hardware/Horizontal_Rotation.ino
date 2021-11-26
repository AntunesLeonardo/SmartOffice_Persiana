/**
 * @file Horizontal_Rotation.ino
 * 
 * @mainpage Hardware control for automated blinds
 * 
 * @section description Description
 * Functions related to motor responsable for horizontal rotation of blinds' blades.
 */

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
