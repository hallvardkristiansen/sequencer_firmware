byte encoder_state = B11111111;
int last_enc_state[] {3, 3, 3, 3};
bool left_primed = false;
bool right_primed = false;

void initialise_mcp() {
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // set pins to input
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(20);
  Wire.beginTransmission(0x20);
  Wire.write(0x02); // set interrupts to enabled
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(20);
  Wire.beginTransmission(0x20);
  Wire.write(0x03); // set defaults to high
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(20);
  Wire.beginTransmission(0x20);
  Wire.write(0x04); // set pins to compare to self
  Wire.write(0x00);
  Wire.endTransmission();
  delay(20);
  Wire.beginTransmission(0x20);
  Wire.write(0x05); // set interrupt to active low
  Wire.write(B00011000);
  Wire.endTransmission();
  delay(20);
  Wire.beginTransmission(0x20);
  Wire.write(0x06); // set pins to pulled high
  Wire.write(0xFF);
  Wire.endTransmission();
}

int encoder_direction(int current_state, int previous_state) {
  int direction = 0;
  if (current_state != previous_state) {
    switch(current_state) {
      // rotation right = 01 -> 00 -> 10
      // rotation left = 10 -> 00 -> 01
      case 0 :
        if (previous_state == 1) {
          right_primed = false;
          left_primed = true;
        }
        if (previous_state == 2) {
          right_primed = true;
          left_primed = false;
        }
        break;
      case 1 :
        if (previous_state == 0) {
          right_primed = true;
          left_primed = false;
        }
        if (previous_state == 3) {
          right_primed = false;
          left_primed = true;
        }
        break;
      case 2 :
        if (previous_state == 0) {
          right_primed = false;
          left_primed = true;
        }
        if (previous_state == 3) {
          right_primed = true;
          left_primed = false;
        }
        break;
      case 3 :
        if (previous_state == 1) {
          if (right_primed) {
            direction = 2;
            enc_modified = true;
          }
        }
        if (previous_state == 2) {
          if (left_primed) {
            direction = 1;
            enc_modified = true;
          }
        }
        break;
    }
  }
  return direction;
}

void read_encoder_values() {
  Wire.beginTransmission(0x20);
  Wire.write(0x08); // ask to read from interrupt register
  Wire.endTransmission();
  Wire.requestFrom(0x20,1); // read data
  encoder_state = Wire.read();

  byte mask_a = B11000000;
  byte mask_b = B00110000;
  byte mask_c = B00001100;
  byte mask_d = B00000011;
  byte val_a = encoder_state & mask_a; // Swing
  byte val_b = encoder_state & mask_b; // Dur
  byte val_c = encoder_state & mask_c; // Mode
  byte val_d = encoder_state & mask_d; // Steps
  val_a = val_a >> 6;
  val_b = val_b >> 4;
  val_c = val_c >> 2;

  int dir_a = encoder_direction(val_a, last_enc_state[0]);
  int dir_b = encoder_direction(val_b, last_enc_state[1]);
  int dir_c = encoder_direction(val_c, last_enc_state[2]);
  int dir_d = encoder_direction(val_d, last_enc_state[3]);

  switch(dir_a) {
    case 2 :
      enc_swing_mod = -1;
    break;
    case 1 :
      enc_swing_mod = 1;
    break;
  }
  switch(dir_b) {
    case 1 :
      enc_dur_mod = -1;
    break;
    case 2 :
      enc_dur_mod = 1;
    break;
  }
  switch(dir_c) {
    case 1 :
      enc_mode_mod = -1;
    break;
    case 2 :
      enc_mode_mod = 1;
    break;
  }
  switch(dir_d) {
    case 2 :
      enc_steps_mod = -1;
    break;
    case 1 :
      enc_steps_mod = 1;
    break;
  }

  last_enc_state[0] = val_a;
  last_enc_state[1] = val_b;
  last_enc_state[2] = val_c;
  last_enc_state[3] = val_d;
}
