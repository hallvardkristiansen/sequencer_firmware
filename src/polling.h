void poll_btns() {
  if (polling) {
    btn_mode_state = !digitalRead(btn_mode_pin);
    btn_steps_state = !digitalRead(btn_steps_pin);
    btn_swing_state = !digitalRead(btn_swing_pin);
    btn_dur_state = !digitalRead(btn_dur_pin);
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
    refresh_trellis = true;
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
  if (btn_mode_state != btn_mode_down) {
    btn_mode_down = btn_mode_state;
    mode_press();
    refresh_trellis = true;
  }
  if (btn_steps_state != btn_steps_down) {
    btn_steps_down = btn_steps_state;
    steps_press();
    refresh_trellis = true;
  }
  if (btn_swing_state != btn_swing_down) {
    btn_swing_down = btn_swing_state;
    swing_press();
    refresh_trellis = true;
  }
  if (btn_dur_state != btn_dur_down) {
    btn_dur_down = btn_dur_state;
    dur_press();
    refresh_trellis = true;
  }

  if (enc_mode_mod != 0) {
    mode_rotate();
    enc_mode_mod = 0;
    refresh_trellis = true;
  }
  if (enc_steps_mod != 0) {
    steps_rotate();
    enc_steps_mod = 0;
    refresh_trellis = true;
  }
  if (enc_swing_mod != 0) {
    swing_rotate();
    enc_swing_mod = 0;
    refresh_trellis = true;
  }
  if (enc_dur_mod != 0) {
    dur_rotate();
    enc_dur_mod = 0;
    refresh_trellis = true;
  }
}

void update_timers() {
  millitime = millis();
  microtime = micros();

  polling = (millitime - last_polltime) >= poll_hz;
  perform_save = (millitime - last_save_time) >= save_hz;
  update_spi_dacs = (microtime - last_spi_dac_update) >= spi_dac_hz;
  update_int_dacs = (microtime - last_int_dac_update) >= int_dac_hz;
  adc_poll = (microtime - last_int_adc_update) >= int_adc_hz;

  if (polling || last_polltime > millitime) {
    last_polltime = millitime;
  }
  if (perform_save || last_save_time > millitime) {
    last_save_time = millitime;
  }
  if (update_spi_dacs || last_spi_dac_update > microtime) {
    last_spi_dac_update = microtime;
  }
  if (update_int_dacs || last_int_dac_update > microtime) {
    last_int_dac_update = microtime;
  }
  if (adc_poll || last_int_adc_update > microtime) {
    last_int_adc_update = microtime;
  }

  triggering = (microtime - last_clock_time) < trigger_dur;
  syncing = (microtime - last_clock_time) < sync_dur;
}
