void poll_btns() {
  if (polling) {
    bool mode_state = !digitalRead(btn_mode_pin);
    if (mode_state != btn_mode_down) {
      btn_mode_down = mode_state;
      if (mode_state) {
        Serial.println("mode down");
      } else {
        Serial.println("mode up");
        enc_modified = false;
      }
    }

    bool steps_state = !digitalRead(btn_steps_pin);
    if (steps_state != btn_steps_down) {
      btn_steps_down = steps_state;
      if (steps_state) {
        Serial.println("steps down");
      } else {
        Serial.println("steps up");
        if (!enc_modified) {
          paused = !paused;
        }
        enc_modified = false;
      }
    }

    bool swing_state = !digitalRead(btn_swing_pin);
    if (swing_state != btn_swing_down) {
      btn_swing_down = swing_state;
      if (swing_state) {
        Serial.println("swing down");
      } else {
        Serial.println("swing up");
        enc_modified = false;
      }
    }

    bool dur_state = !digitalRead(btn_dur_pin);
    if (dur_state != btn_dur_down) {
      btn_dur_down = dur_state;
      if (dur_state) {
        Serial.println("dur down");
      } else {
        Serial.println("dur up");
        enc_modified = false;
      }
    }
  }
}

void poll_encoders() {
  if(!digitalRead(ioexp_int_pin)){
    read_encoder_values();
  }
}

void poll_clock() {
  if (!paused) {
    if (!digitalRead(clock_pin) && !triggered){
      last_clock_time = looptime;
      triggered = true;
      all_out = true;
      increment_sequence(1);
    } else if (digitalRead(clock_pin) && triggered) {
      triggered = false;
    }
  }
  if (polling) {
    update_dacs = true;
  }
}

void poll_rst() {
  if(!digitalRead(rst_pin) && !reset && !paused){
    increment_sequence(99);
    reset = true;
  } else if (digitalRead(rst_pin) && reset) {
    reset = false;
  }
}

void sync_keypad() {
  if (polling) {
    trellis.read();
    if (!i2c_busy && refresh_trellis) {
      refresh_keypad_colours();
      trellis.pixels.show();
      refresh_trellis = false;
    }
    last_looptime = looptime;
  }
}

void resolve_interactions() {
  if (enc_mode_mod != 0) {
    change_pointers_count(enc_mode_mod);
    increment_sequence(0);
    enc_mode_mod = 0;
  }
  if (enc_steps_mod != 0) {
    if (btn_steps_down) {
      update_pointer(enc_steps_mod); // swap for insert
    } else if (keypad_down) {
      increment_note(enc_steps_mod);
    } else {
      last_clock_time = looptime;
      triggered = true;
      increment_sequence(enc_steps_mod);
    }
    enc_steps_mod = 0;
  } else {
    if (btn_steps_down && btn_mode_down) {
      triggered = false;
      increment_sequence(99);
      reset = true;
    }
  }
  if (enc_swing_mod != 0) {
    if (keypad_down) {
      increment_key_swing(enc_swing_mod);
    } else {
      increment_swing(enc_swing_mod);
    }
    enc_swing_mod = 0;
  }
  if (enc_dur_mod != 0) {
    if (keypad_down) {
      increment_key_glide(enc_dur_mod);
    } else {
      increment_glide(enc_dur_mod);
    }
    enc_dur_mod = 0;
  }
}

void update_timers() {
  polling = (looptime - last_looptime) >= poll_hz;
}
