void poll_btns() {
  if (polling_btns) {
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
  if (refresh_trellis && polling_keys) {
    refresh_keypad_colours();
    trellis.pixels.show();
    refresh_trellis = false;
  }
}

void poll_clock() {
  if (!digitalRead(clock_pin) && !triggered){
    triggered = true;
    fire_trigger();
    refresh_trellis = true;
  } else if (digitalRead(clock_pin) && triggered) {
    triggered = false;
  }
}

void poll_rst() {
  if(!digitalRead(rst_pin) && !reset){
    reset = true;
    fire_reset();
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

  polling_keys = (millitime - last_key_polltime) >= key_poll_hz;
  last_key_polltime = polling_keys ? millitime : last_key_polltime;

  polling_btns = (millitime - last_btn_polltime) >= btn_poll_hz;
  last_btn_polltime = polling_btns ? millitime : last_btn_polltime;

  perform_save = (millitime - last_save_time) >= save_hz;
  last_save_time = perform_save ? millitime : last_save_time;

  print_debug = (millitime - last_print_time) >= debug_serial_hz;
  last_print_time = print_debug ? millitime : last_print_time;

  update_spi_dacs = (microtime - last_spi_dac_update) >= spi_dac_hz;
  last_spi_dac_update = update_spi_dacs ? microtime : last_spi_dac_update;

  update_int_dacs = (microtime - last_int_dac_update) >= int_dac_hz;
  last_int_dac_update = update_int_dacs ? microtime : last_int_dac_update;

  adc_poll = (microtime - last_int_adc_update) >= int_adc_hz;
  last_int_adc_update = adc_poll ? microtime : last_int_adc_update;

  triggering = microtime >= last_clock_time && (microtime - last_clock_time) < trigger_dur;
  syncing = (microtime - last_sync_time) < sync_dur;

  btn_hold_primed = millitime - last_btn_press > btn_hold_wait;
  keypad_mode_menu = millitime - last_enc_action < temp_menu_dur;
  menu_mode_active = btn_mode_down && btn_hold_primed;
  menu_steps_active = btn_steps_down && btn_hold_primed;
  menu_swing_active = keypad_mode_menu ? menu_swing_active : false;
  menu_dur_active = keypad_mode_menu ? menu_dur_active : false;
  refresh_trellis = (!refresh_trellis && !keypad_mode_menu && (menu_swing_active || menu_dur_active)) ? true : refresh_trellis;
}
