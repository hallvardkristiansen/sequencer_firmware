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
  if (print_debug) {
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
    Serial.println();
    Serial.print("mode_btn: ");
    Serial.print(btn_mode_down);
    Serial.println();
    Serial.print("mode menu active: ");
    Serial.print(menu_mode_active);
    Serial.println();
    Serial.print("button primed: ");
    Serial.print(btn_hold_primed);
    Serial.println();
    Serial.print("steps_btn: ");
    Serial.print(btn_steps_down);
    Serial.println();
    Serial.print("swing_btn: ");
    Serial.print(btn_swing_down);
    Serial.println();
    Serial.print("dur_btn: ");
    Serial.print(btn_dur_down);
    Serial.println();
    Serial.println();
    Serial.print("enc_modified: ");
    Serial.print(enc_modified);
    Serial.println();
  }
}
