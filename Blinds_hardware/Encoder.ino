/**
 * @file Encoder.ino
 * 
 * @mainpage Hardware control for automated blinds
 * 
 * @section description Description
 * Encoder related functions.
 */

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
