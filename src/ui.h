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
  if (all_btns_pressed) {
    clear_state();
    intro_animation();
  }
  all_btns_pressed = false;
}

void change_playback_mode(int amnt) {
  if (playback_mode < 3) {
    playback_mode += amnt;
  } else {
    playback_mode = 0;
  }
  if (playback_mode == 0) {
    incrementor = 1;
  } else if (playback_mode == 2) {
    incrementor = -1;
  }
}

void btn_press(int which) {
  bool btn_down = btn_mode_down || btn_steps_down || btn_swing_down || btn_dur_down;
  if (btn_down) {
    prime_btn_hold();
    enc_modified = false;
  } else {
    if (!enc_modified) {
      all_hold_release();
      switch (which) {
        case 0: // mode
          if (!btn_steps_down && !btn_swing_down && !btn_dur_down && !btn_hold_primed) {
            if (copy_section[1] == 0) {
              copy_page();
              Serial.println("Page copied");
            } else {
              paste_page();
              Serial.println("Page pasted");
            }
          } else {
            if (btn_steps_down && !btn_swing_down && !btn_dur_down) {
              fire_reset();
            }
            if (!btn_steps_down && !btn_swing_down && btn_dur_down) {
              clear_page();
              Serial.println("Page cleared, mode btn triggered");
            }
          }
        break;
        case 1: // steps
          if (copy_section[1] > 0) {
            fill_active_pattern();
          } else {
            if (!menu_steps_active && !btn_mode_down && !btn_swing_down && !btn_dur_down) {
              paused = !paused;
              pattern_ended = false;
            }
            if (btn_mode_down && !btn_swing_down && !btn_dur_down) {
              fire_reset();
            }
          }
          menu_steps_active = false;
        break;
        case 2: // swing
          if (!btn_mode_down && !btn_steps_down && !btn_dur_down) {
            randomize_page();
          }
        break;
        case 3: // dur
          if (!btn_mode_down && !btn_steps_down && !btn_swing_down) {
            recording_cv = !recording_cv;
          } if (btn_mode_down && !btn_steps_down && !btn_swing_down) {
            clear_page();
            Serial.println("Page cleared, dur btn triggered");
          }
        break;
      }
    } else {
      if (btn_steps_down) {
        update_pointer(0);
      }
    }
    enc_modified = false;
    refresh_trellis = true;
  }
}

void enc_rotate(int which) {
  switch (which) {
    case 0: // mode
      change_page(enc_mode_mod);
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
      if (btn_steps_down) {
        menu_steps_active = true;
        change_pattern_start(enc_swing_mod);
      } else {
        if (keypad_down) {
          increment_key_swing(enc_swing_mod);
        } else {
          last_enc_action = millitime;
          menu_swing_active = true;
          menu_semitones_active = false;
          menu_dur_active = false;
          increment_swing(enc_swing_mod);
        }
      }
    break;
    case 3: // dur
      if (btn_steps_down) {
        menu_steps_active = true;
        change_pattern_length(enc_dur_mod);
      } else {
        if (keypad_down) {
          increment_key_glide(enc_dur_mod);
        } else {
          last_enc_action = millitime;
          menu_dur_active = true;
          menu_swing_active = false;
          menu_semitones_active = false;
          increment_glide(enc_dur_mod);
        }
      }
    break;
  }
  refresh_trellis = true;
}

void keypad_pressed(int key_num) {
  keypads_down[key_num] = true;
  keypad_down = true;
  last_keypad_down_index = (grid_size * current_page) + key_num;
}
void keypad_released(int key_num) {
  keypads_down[key_num] = false;
  keypad_down = false;
  menu_semitones_active = false;
  int pattern_index = (grid_size * current_page) + key_num;

  if (menu_mode_active) {
    switch (key_num) {
      case 0:
        incrementor = -1;
        playback_mode = 0;
      break;
      case 1:
        playback_mode = 1;
      break;
      case 2:
        incrementor = 1;
        playback_mode = 2;
      break;
      case 3:
        trigger_mode = !trigger_mode;
      break;
      case 4:
        pointers = 1;
      break;
      case 5:
        pointers = 2;
      break;
      case 6:
        pointers = 4;
      break;
    }
  } else if (menu_steps_active) {
    pattern_length = (grid_size * key_num) - pattern_start;
  } else if (menu_swing_active) {
    global_swing = key_num;
  } else if (menu_dur_active) {
    glide_mode = key_num;
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
  double redval = pattern_tone[(current_page * grid_size) + num] + 5;
  double greenval = redval - 40 > 0 ? redval - 40 : 0;
  int blueval = current_page == num ? 2 : 0;
  uint32_t returnval = trellis.pixels.Color(redval, greenval, blueval);
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
      switch (i) {
        case 0:
          trellis.pixels.setPixelColor(i, 0x991111);
        break;
        case 1:
          trellis.pixels.setPixelColor(i, 0x119911);
        break;
        case 2:
          trellis.pixels.setPixelColor(i, 0x111199);
        break;
        case 3:
          trellis.pixels.setPixelColor(i, 0x555500);
        break;
        case 4:
          trellis.pixels.setPixelColor(i, 0x999911);
        break;
        case 5:
          trellis.pixels.setPixelColor(i, 0x991199);
        break;
        case 6:
          trellis.pixels.setPixelColor(i, 0x119999);
        break;
        default:
          trellis.pixels.setPixelColor(i, 0x000001);
        break;
      }
    }
  } else if (menu_steps_active) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < (pattern_start + pattern_length) / grid_size && i >= pattern_start / grid_size) {
        trellis.pixels.setPixelColor(i, 0x111199);
      } else {
        trellis.pixels.setPixelColor(i, 0x000001);
      }
    }
  } else if (menu_swing_active) {
    int swing = get_swing();
    int color_step = floor(swing / grid_size);
    int relative_step = swing % grid_size;

    uint32_t active_color = Wheel(color_step * grid_size);
    uint32_t inactive_color = 0x010101;
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < relative_step) {
        trellis.pixels.setPixelColor(i, active_color);
      } else {
        trellis.pixels.setPixelColor(i, inactive_color);
      }
    }
  } else if (menu_dur_active) {
    int glide = get_glide();
    int color_step = floor(glide / grid_size);
    int relative_step = glide % grid_size;

    uint32_t active_color = Wheel(color_step * grid_size);
    uint32_t inactive_color = 0x010100;
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < relative_step) {
        trellis.pixels.setPixelColor(i, active_color);
      } else {
        trellis.pixels.setPixelColor(i, inactive_color);
      }
    }
  } else if (menu_semitones_active) {
    float color_step = floor(pattern_tone[last_keypad_down_index] / grid_size);
    int relative_step = pattern_tone[last_keypad_down_index] % grid_size;

    uint32_t active_color = Wheel(color_step * grid_size);
    uint32_t inactive_color = 0x000001;
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < relative_step) {
        trellis.pixels.setPixelColor(i, active_color);
      } else {
        trellis.pixels.setPixelColor(i, inactive_color);
      }
    }
  } else {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      int pattern_index = (current_page * grid_size) + i;
      uint32_t active_color = is_pointer(i) ? 0xAAAAAA : keypad_color(i);
      uint32_t inactive_color = (i >= (pattern_start / grid_size)) && (i < ((pattern_start + pattern_length) / grid_size)) ? 0x000001 : 0x000000;
      active_color = i == current_page ? 0xAAAAAF : active_color;
      inactive_color = copy_section[1] > 0 ? 0x020001 : inactive_color;
      inactive_color = i == current_page ? 0x000011 : inactive_color;
      uint32_t trigger_color = i == current_page ? 0x221111 : 0x221100;
      uint32_t pointer_color = i == current_page ? 0x111122 : 0x111111;
      uint32_t cv_color = keypad_color(i);
      if (pattern_on[pattern_index]) {
        if (keypads_down[i]) {
          trellis.pixels.setPixelColor(i, cv_color);
        } else if (is_pointer(i)) {
          trellis.pixels.setPixelColor(i, active_color);
        } else if (trigger_mode) {
          trellis.pixels.setPixelColor(i, trigger_color);
        } else {
          trellis.pixels.setPixelColor(i, cv_color);
        }
      } else {
        if (keypads_down[i]) {
          trellis.pixels.setPixelColor(i, cv_color);
        } else if (is_pointer(i)) {
          trellis.pixels.setPixelColor(i, pointer_color);
        } else {
          trellis.pixels.setPixelColor(i, inactive_color);
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
