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
      refresh_trellis = true;
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
      refresh_trellis = true;
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
      refresh_trellis = true;
    }

    bool dur_state = !digitalRead(btn_dur_pin);
    if (dur_state != btn_dur_down) {
      btn_dur_down = dur_state;
      if (dur_state) {
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
      refresh_trellis = true;
    }
  }
}

void poll_encoders() {
  if (!digitalRead(ioexp_int_pin)) {
    read_encoder_values();
  }
}

void poll_keypad() {
  if (!digitalRead(trellis_int_pin)) {
    trellis.read();
  }
  if (refresh_trellis && polling) {
    refresh_keypad_colours();
    trellis.pixels.show();
    refresh_trellis = false;
  }
}

void poll_clock() {
  if (!paused) {
    if (!digitalRead(clock_pin) && !triggered){
      fire_trigger();
      increment_sequence(1);
      refresh_trellis = true;
    } else if (digitalRead(clock_pin) && triggered) {
      triggered = false;
    }
  }
}

void poll_rst() {
  if(!digitalRead(rst_pin) && !reset && !paused){
    fire_reset();
    increment_sequence(0);
    refresh_trellis = true;
  } else if (digitalRead(rst_pin) && reset) {
    reset = false;
  }
}

void poll_cv() {
  if (adc_poll) {
    cv_mode = analogRead(cv_mode_pin);
    cv_steps = analogRead(cv_steps_pin);
    cv_swing = analogRead(cv_swing_pin);
    cv_dur = analogRead(cv_dur_pin);
  }
}

void poll_inputs() {
  poll_clock();
  poll_rst();
  poll_cv();
}

void poll_ui() {
  poll_btns();
  poll_encoders();
  poll_keypad();
}

void resolve_interactions() {
  if (enc_mode_mod != 0) {
    change_pointers_count(enc_mode_mod);
    increment_sequence(0);
    enc_mode_mod = 0;
    refresh_trellis = true;
  }
  if (enc_steps_mod != 0) {
    if (btn_steps_down) {
      update_pointer(enc_steps_mod); // swap for insert
    } else if (keypad_down) {
      increment_note(enc_steps_mod);
    } else {
      increment_sequence(enc_steps_mod);
    }
    enc_steps_mod = 0;
    refresh_trellis = true;
  } else {
    if (btn_steps_down && btn_mode_down) {
      fire_reset();
      refresh_trellis = true;
    }
  }
  if (enc_swing_mod != 0) {
    if (keypad_down) {
      increment_key_swing(enc_swing_mod);
    } else {
      increment_swing(enc_swing_mod);
    }
    enc_swing_mod = 0;
    refresh_trellis = true;
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
    refresh_trellis = true;
  }
}

void update_timers() {
  millitime = millis();
  microtime = micros();

  polling = (millitime - last_polltime) >= poll_hz;
  if (polling || last_polltime > millitime) {
    last_polltime = millitime; // this will cause extra triggers on overflow
  }

  update_spi_dacs = (microtime - last_spi_dac_update) >= spi_dac_hz;
  update_int_dacs = (microtime - last_int_dac_update) >= int_dac_hz;
  adc_poll = (microtime - last_int_adc_update) >= int_adc_hz;

  if (update_spi_dacs || last_spi_dac_update > microtime) {
    last_spi_dac_update = microtime; // this may cause extra triggers on overflow
  }
  if (update_int_dacs || last_int_dac_update > microtime) {
    last_int_dac_update = microtime; // this may cause extra triggers on overflow
  }
  if (adc_poll || last_int_adc_update > microtime) {
    last_int_adc_update = microtime;
  }

  if (swinging && global_swing > 0) {
    triggering = (microtime - last_clock_time) < (trigger_dur + (global_swing * swing_dur))  && (microtime - last_clock_time) > (global_swing * swing_dur);
  } else {
    triggering = (microtime - last_clock_time) < trigger_dur;
  }

  syncing = (microtime - last_clock_time) < sync_dur;
  if (sync_out && !syncing) {
    sync_out = false;
  }
  if (all_out && !syncing) {
    all_out = false;
  }

  perform_save = (millitime - last_save_time) >= save_hz;
  if (perform_save || last_save_time > millitime) {
    last_save_time = millitime;
  }
}
