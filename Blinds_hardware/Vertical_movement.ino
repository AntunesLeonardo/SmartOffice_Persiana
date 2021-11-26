/**
 * @file Vertical_movement.ino
 * 
 * @mainpage Hardware control for automated blinds
 * 
 * @section description Description
 * Functions related to motor responsable vertical movement of the blinds.
 */


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
