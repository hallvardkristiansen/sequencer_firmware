void intro_animation() {
  for (int j=0; j<=2; j++) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      trellis.pixels.setPixelColor(i, 0x900000);
      trellis.pixels.show();
      delay(5);
    }
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      trellis.pixels.setPixelColor(i, 0x090000);
      trellis.pixels.show();
      delay(5);
    }
  }
}

void prime_btn_hold() {
  last_btn_press = millitime;
  all_btns_pressed = (btn_mode_down && btn_steps_down && btn_swing_down && btn_dur_down);
}

void all_hold_release() {
  if (all_btns_pressed && btn_hold_primed) {
    clear_state();
    intro_animation();
  }
  all_btns_pressed = false;
}

void btn_press(int which) {
  bool btn_down = btn_mode_down || btn_steps_down || btn_swing_down || btn_dur_down;
  if (btn_down) {
    prime_btn_hold();
  } else {
    if (!enc_modified) {
      all_hold_release();
      switch (which) {
        case 0: // mode
          if (btn_steps_down) {
            fire_reset();
          }
          if (!btn_steps_down && !btn_swing_down && !btn_dur_down) {
            if (playback_mode < 3) {
              playback_mode++;
            } else {
              playback_mode = 0;
            }
            if (playback_mode == 0) {
              incrementor = 1;
            } else if (playback_mode == 2) {
              incrementor = -1;
            }
          }
        break;
        case 1: // steps
          if (btn_mode_down) {
            fire_reset();
          }
          if (!btn_mode_down && !btn_swing_down && !btn_dur_down) {
            paused = !paused;
            pattern_ended = false;
          }
        break;
        case 2: // swing
        break;
        case 3: // dur
          if (glide_mode + 1 > glide_modes) {
            glide_mode = 0;
          } else {
            glide_mode++;
          }
        break;
      }
    }
    enc_modified = false;
    refresh_trellis = true;
  }
}

void enc_rotate(int which) {
  switch (which) {
    case 0: // mode
      change_pointers_count(enc_mode_mod);
    break;
    case 1: // steps
      if (keypad_down) {
        menu_semitones_active = true;
        menu_swing_active = false;
        menu_dur_active = false;
        increment_note(enc_steps_mod);
      } else {
        update_pointer(enc_steps_mod);
        menu_swing_active = false;
        menu_semitones_active = false;
        menu_dur_active = false;
      }
    break;
    case 2: // swing
      if (keypad_down) {
        increment_key_swing(enc_swing_mod);
      } else {
        last_enc_action = millitime;
        menu_swing_active = true;
        menu_semitones_active = false;
        menu_dur_active = false;
        increment_swing(enc_swing_mod);
      }
    break;
    case 3: // dur
      if (keypad_down) {
        increment_key_glide(enc_dur_mod);
      } else {
        last_enc_action = millitime;
        menu_dur_active = true;
        menu_swing_active = false;
        menu_semitones_active = false;
        increment_glide(enc_dur_mod);
      }
      if (btn_steps_down) {
        change_pattern_length(enc_dur_mod);
      }
    break;
  }
  refresh_trellis = true;
}

void keypad_pressed(int key_num) {
  keypads_down[key_num] = true;
  keypad_down = true;
  last_keypad_down_index = (grid_size * current_page) + key_num;
  prime_btn_hold();
}
void keypad_released(int key_num) {
  keypads_down[key_num] = false;
  keypad_down = false;
  menu_semitones_active = false;
  int pattern_index = (grid_size * current_page) + key_num;

  if (menu_steps_active) {
    pattern_length = grid_size * key_num;
  } else if (menu_swing_active) {
    global_swing = key_num;
  } else if (menu_dur_active) {
    global_glide = key_num;
  } else {
    if (!pattern_on[pattern_index]) {
      pattern_on[pattern_index] = true;
    } else if (!enc_modified && pattern_on[pattern_index]) {
      pattern_on[pattern_index] = false;
    }
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

TrellisCallback keypress(keyEvent evt){
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    keypad_pressed(evt.bit.NUM);
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    keypad_released(evt.bit.NUM);
  }
  return 0;
}

void trellis_keypress_events() {
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, keypress);
  }
}

void refresh_keypad_colours() {
  if (menu_mode_active) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      trellis.pixels.setPixelColor(i, 0x119911);
    }
  } else if (menu_steps_active) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < pattern_length / grid_size) {
        trellis.pixels.setPixelColor(i, 0x111199);
      } else {
        trellis.pixels.setPixelColor(i, 0x000001);
      }
    }
  } else if (menu_swing_active) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < global_swing) {
        trellis.pixels.setPixelColor(i, 0x999911);
      } else {
        trellis.pixels.setPixelColor(i, 0x010100);
      }
    }
  } else if (menu_dur_active) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < global_glide) {
        trellis.pixels.setPixelColor(i, 0x991199);
      } else {
        trellis.pixels.setPixelColor(i, 0x010001);
      }
    }
  } else if (menu_semitones_active) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < pattern_tone[last_keypad_down_index]) {
        trellis.pixels.setPixelColor(i, 0xff1166);
      } else {
        trellis.pixels.setPixelColor(i, 0x010001);
      }
    }
  } else {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      int pattern_index = (current_page * grid_size) + i;
      if (pattern_on[pattern_index]) {
        if (keypads_down[i]) {
          trellis.pixels.setPixelColor(i, keypad_color(i));
        } else if (is_pointer(i)) {
          trellis.pixels.setPixelColor(i, 0xAAAAAA);
        } else if (pointers == 4) {
          trellis.pixels.setPixelColor(i, 0x111100);
        } else {
          trellis.pixels.setPixelColor(i, keypad_color(i));
        }
      } else {
        if (keypads_down[i]) {
          trellis.pixels.setPixelColor(i, keypad_color(i));
        } else if (is_pointer(i)) {
          trellis.pixels.setPixelColor(i, 0x111111);
        } else {
          trellis.pixels.setPixelColor(i, 0x000001);
        }
      }
    }
  }
}

void initialise_trellis() {
  trellis.begin();
  trellis_keypress_events();
  intro_animation();
}
