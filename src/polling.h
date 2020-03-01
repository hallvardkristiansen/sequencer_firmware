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
    forward_clock = true;
    if (recording_cv) {
      sample_cv = true;
    }
  } else if (digitalRead(clock_pin) && triggered) {
    triggered = false;
  }
}

void poll_rst() {
  if(!digitalRead(rst_pin) && !reset){
    reset = true;
    fire_reset();
  } else if (digitalRead(rst_pin) && reset) {
    reset = false;
  }
}

int cv_to_semitone(int value) {
  return min(60, max(0, 60 - (value / 10)));
}

void poll_cv() {
  if (adc_poll) {
    cv_mode = analogRead(cv_mode_pin);
    cv_steps = analogRead(cv_steps_pin);
    cv_swing = analogRead(cv_swing_pin);
    cv_dur = analogRead(cv_dur_pin);
    if (sample_cv) {
      int steplength = grid_size / pointers;
      int cvs[4] = {cv_mode, cv_steps, cv_swing, cv_dur};
      for (int i = 0; i < pointers; i++) {
        int this_pointer = pattern_pointer + (steplength * i);
        switch (pointers) {
          case 1:
            pattern_tone[this_pointer] = cv_to_semitone(cv_mode);
            pattern_on[this_pointer] = cv_steps < 200;
            pattern_swing[this_pointer] = cv_to_semitone(cv_swing);
            pattern_glide[this_pointer] = cv_to_semitone(cv_dur);
          break;
          case 2:
            if (i == 0) {
              pattern_tone[this_pointer] = cv_to_semitone(cvs[0]);
              pattern_on[this_pointer] = cvs[1] < 200;
            } else {
              pattern_tone[this_pointer] = cv_to_semitone(cvs[2]);
              pattern_on[this_pointer] = cvs[3] < 200;
            }
          break;
          case 4:
            if (trigger_mode) {
              pattern_on[this_pointer] = cvs[i] < 200;
            } else {
              pattern_tone[this_pointer] = cv_to_semitone(cvs[i]);
            }
          break;
        }
      }
      sample_cv = false;
    }
  }
}

void poll_inputs() {
  poll_rst();
  poll_cv();
  poll_clock();
}

void poll_ui() {
  poll_btns();
  poll_encoders();
  poll_keypad();
}

void resolve_interactions() {
  if (btn_mode_state != btn_mode_down) {
    btn_mode_down = btn_mode_state;
    btn_press(0);
  }
  if (btn_steps_state != btn_steps_down) {
    btn_steps_down = btn_steps_state;
    btn_press(1);
  }
  if (btn_swing_state != btn_swing_down) {
    btn_swing_down = btn_swing_state;
    btn_press(2);
  }
  if (btn_dur_state != btn_dur_down) {
    btn_dur_down = btn_dur_state;
    btn_press(3);
  }

  if (enc_mode_mod != 0) {
    enc_rotate(0);
    enc_mode_mod = 0;
  }
  if (enc_steps_mod != 0) {
    enc_rotate(1);
    enc_steps_mod = 0;
  }
  if (enc_swing_mod != 0) {
    enc_rotate(2);
    enc_swing_mod = 0;
  }
  if (enc_dur_mod != 0) {
    enc_rotate(3);
    enc_dur_mod = 0;
  }
}

bool allow_ui() {
  return !(triggering || syncing);
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

  ui_blink = (millitime - last_blink_time) >= blink_dur;
  last_blink_time = ui_blink ? millitime : last_blink_time;
  blinker = ui_blink ? !blinker : blinker;

  print_debug = (millitime - last_print_time) >= debug_serial_hz;
  last_print_time = print_debug ? millitime : last_print_time;

  update_spi_dacs = (microtime - last_spi_dac_update) >= spi_dac_hz;
  last_spi_dac_update = update_spi_dacs ? microtime : last_spi_dac_update;

  update_int_dacs = (microtime - last_int_dac_update) >= int_dac_hz;
  last_int_dac_update = update_int_dacs ? microtime : last_int_dac_update;

  adc_poll = (microtime - last_int_adc_update) >= int_adc_hz;
  last_int_adc_update = adc_poll ? microtime : last_int_adc_update;

  triggering = microtime >= (last_clock_time + swing_delay) && (microtime - (last_clock_time + swing_delay)) < trigger_dur;
  syncing = (microtime - last_sync_time) < sync_dur;

  btn_hold_primed = millitime - last_btn_press > btn_hold_wait;
  keypad_mode_menu = millitime - last_enc_action < temp_menu_dur;
  menu_mode_active = btn_mode_down && btn_hold_primed;
  menu_steps_active = btn_steps_down && btn_hold_primed;
  refresh_trellis = ((menu_mode_active || menu_steps_active || menu_dur_active || menu_swing_active) && !keypad_mode_menu) ? true : refresh_trellis;
  menu_swing_active = keypad_mode_menu ? menu_swing_active : false;
  menu_dur_active = keypad_mode_menu ? menu_dur_active : false;
}
