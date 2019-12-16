void scan_for_i2c_addresses() {
  byte count = 0;
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0) {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
    } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}

void serial_print_debug() {
  Serial.println();
  Serial.print("cv_mode: ");
  Serial.print(cv_mode);
  Serial.println();
  Serial.print("cv_steps: ");
  Serial.print(cv_steps);
  Serial.println();
  Serial.print("cv_swing: ");
  Serial.print(cv_swing);
  Serial.println();
  Serial.print("cv_dur: ");
  Serial.print(cv_dur);
  Serial.println();
  Serial.println();
  Serial.print("pattern_ended: ");
  Serial.print(pattern_ended);
  Serial.println();
  Serial.print("current_page: ");
  Serial.print(current_page);
  Serial.println();
  Serial.print("pointer: ");
  Serial.print(pointer);
}
