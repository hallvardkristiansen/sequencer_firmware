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
        if (!enc_modified) {
          if (glide_mode + 1 >= glide_modes) {
            glide_mode = 0;
          } else {
            glide_mode++;
          }
        }
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
      last_clock_time = signaltime;
      triggered = true;
      all_out = true;
      swinging = !swinging;
      increment_sequence(1);
    } else if (digitalRead(clock_pin) && triggered) {
      triggered = false;
    }
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
    refresh_keypad_colours();
    trellis.pixels.show();
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
    if (btn_steps_down) {
      change_pattern_length(enc_dur_mod);
    }
    enc_dur_mod = 0;
  }
}

void update_timers() {
  looptime = millis();
  signaltime = micros();
  polling = (looptime - last_looptime) >= poll_hz;
  if (polling || last_looptime > looptime) {
    last_looptime = looptime; // this will cause extra triggers on overflow
  }
  update_spi_dacs = (signaltime - last_signaltime) >= dac_hz;
  update_int_dacs = (signaltime - last_signaltime) >= dac_hz; // test if this works
  if (update_spi_dacs || last_signaltime > signaltime) {
    last_signaltime = signaltime; // this will cause extra triggers on overflow
  }
  apply_modifiers = (signaltime - last_clock_time) < mod_dur;
  if (swinging && swing_amnt > 0) {
    triggering = (signaltime - last_clock_time) < (trigger_dur + (swing_amnt * swing_dur))  && (signaltime - last_clock_time) > (swing_amnt * swing_dur);
  } else {
    triggering = (signaltime - last_clock_time) < trigger_dur;
  }
  if (sync_out && !triggering) {
    sync_out = false;
  }
  if (all_out && !triggering) {
    all_out = false;
  }
}
