void mode_press() {
  if (btn_mode_down) {
    Serial.println("mode down");
  } else {
    Serial.println("mode up");
    if (btn_steps_down) {
      fire_reset();
    }
    if (!btn_steps_down && !btn_swing_down && !btn_dur_down) {
      if (incrementor == 1) {
        incrementor = -1;
      } else {
        incrementor = 1;
      }
    }
    enc_modified = false;
  }
}
void steps_press() {
  if (btn_steps_down) {
    Serial.println("steps down");
  } else {
    Serial.println("steps up");
    if (btn_mode_down) {
      fire_reset();
    }
    if (!btn_mode_down && !btn_swing_down && !btn_dur_down) {
      paused = !paused;
    }
    enc_modified = false;
  }
}
void swing_press() {
  if (btn_swing_down) {
    Serial.println("swing down");
  } else {
    Serial.println("swing up");
    enc_modified = false;
  }
}
void dur_press() {
  if (btn_dur_down) {
    Serial.println("dur down");
  } else {
    Serial.println("dur up");
    if (!enc_modified) {
      if (glide_mode + 1 > glide_modes) {
        glide_mode = 0;
      } else {
        glide_mode++;
      }
      Serial.println(glide_mode);
    }
    enc_modified = false;
  }
}

void mode_rotate() {
  change_pointers_count(enc_mode_mod);
}
void steps_rotate() {
  if (keypad_down) {
    increment_note(enc_steps_mod);
  } else {
    update_pointer(enc_steps_mod);
  }
}
void swing_rotate() {
  if (keypad_down) {
    increment_key_swing(enc_swing_mod);
  } else {
    increment_swing(enc_swing_mod);
  }
}
void dur_rotate() {
  if (keypad_down) {
    increment_key_glide(enc_dur_mod);
  } else {
    increment_glide(enc_dur_mod);
  }
  if (btn_steps_down) {
    change_pattern_length(enc_dur_mod);
  }
}

void keypad_pressed(int key_num) {
  keypads_down[key_num] = true;
  keypad_down = true;
}
void keypad_released(int key_num) {
  keypads_down[key_num] = false;
  keypad_down = false;
  int pattern_index = (grid_size * current_page) + key_num;
  if (!pattern_on[pattern_index]) {
    pattern_on[pattern_index] = true;
  } else if (!enc_modified && pattern_on[pattern_index]) {
    pattern_on[pattern_index] = false;
  }
  enc_modified = false;
}

uint32_t Wheel(byte WheelPos) {
  return trellis.pixels.Color(WheelPos, 0, 0);
  if(WheelPos < 85) {
   return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}
uint32_t keypad_color(int num) {
  double mult = 250 / 60;
  double val = (pattern_tone[(current_page * grid_size) + num] * mult) + 5;
  uint32_t returnval = Wheel((int)val);
  return returnval;
}
