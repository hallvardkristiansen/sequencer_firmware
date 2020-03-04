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
  last_btn_press = microtime;
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
      switch (which) {
        case 0: // mode
          if (!btn_steps_down && !btn_swing_down && !btn_dur_down && !btn_hold_primed) {
            if (copy_section[1] == 0) {
              copy_page();
            } else {
              paste_page();
            }
          } else {
            if (btn_steps_down && !btn_swing_down && !btn_dur_down) {
              fire_reset();
            }
          }
        break;
        case 1: // steps
          if (copy_section[1] > 0) {
            fill_nth_page(2);
          } else {
            if (!menu_steps_active && !btn_mode_down && !btn_swing_down && !btn_dur_down) {
              if (hold_for_sync) {
                holding_for_sync = !holding_for_sync;
                paused = false;
              } else {
                paused = !paused;
              }
            }
            if (btn_mode_down && !btn_swing_down && !btn_dur_down) {
              fire_reset();
            }
          }
          menu_steps_active = false;
        break;
        case 2: // swing
          if (copy_section[1] > 0) {
            fill_nth_page(4);
          } else if (!btn_mode_down && !btn_steps_down && !btn_dur_down) {
            randomize_page();
          }
        break;
        case 3: // dur
          if (copy_section[1]) {
            clear_page();
          } else if (!btn_mode_down && !btn_steps_down && !btn_swing_down) {
            recording_cv = !recording_cv;
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
      if (menu_semitones_active) {
        increment_octave(enc_mode_mod);
      } else {
        change_page(enc_mode_mod);
      }
    break;
    case 1: // steps
      if (menu_semitones_active) {
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
          last_enc_action = microtime;
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
          last_enc_action = microtime;
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
  if (!btn_mode_down && !btn_steps_down && !btn_swing_down && !btn_dur_down) {
    prime_btn_hold();
  }

  if (menu_semitones_active) {
    if (key_num < 12) {
      int relative_semitone = (menu_semitones_octave * 12) + key_num;
      pattern_tone[target_keypad_index] = relative_semitone;
      int key_row = floor((target_keypad_index - (current_page * grid_size)) / gridy);
      if (!is_playing()) {
        last_key_press = microtime;
        triggered_manually = true;
        trigger_dur = 50000;
        for (int i = 0; i < pointers; i++) {
          if (pointers == 1) {
            if (trigger_mode) {
              triggers[i] = true;
            } else {
              notes[0] = relative_semitone;
              swing[0] = global_swing;
              glide[0] = global_glide;
              triggers[0] = false;
              triggers[1] = true;
              notes[2] = relative_semitone;
              swing[2] = global_swing;
              glide[2] = global_glide;
              triggers[2] = false;
              triggers[3] = true;
            }
          }
          if (pointers == 2) {
            if (i == 0) {
              notes[0] = relative_semitone;
              swing[0] = global_swing;
              glide[0] = global_glide;
              triggers[0] = false;
              triggers[1] = key_row < 2;
            }
            if (i == 1) {
              notes[2] = relative_semitone;
              swing[2] = global_swing;
              glide[2] = global_glide;
              triggers[2] = false;
              triggers[3] = key_row >= 2;
            }
          }
          if (pointers == 4) {
            triggers[i] = (key_row == i) && trigger_mode;
            notes[i] = relative_semitone;
            swing[i] = global_swing;
            glide[i] = global_glide;
          }
        }
      }
    } else {
      menu_semitones_octave = key_num - 11;
    }
  }
}
void keypad_released(int key_num) {
  keypads_down[key_num] = false;
  keypad_down = false;
  triggered_manually = false;
  trigger_dur = 5000;
  int pattern_index = (grid_size * current_page) + key_num;

  if (btn_hold_primed && !btn_mode_down && !btn_steps_down && !btn_swing_down && !btn_dur_down) {
    if (!menu_semitones_active) {
      target_keypad_index = pattern_index;
      menu_semitones_active = true;
      menu_dur_active = false;
      menu_mode_active = false;
      menu_steps_active = false;
      menu_swing_active = false;
    } else {
      menu_semitones_active = false;
    }
    menu_semitones_octave = floor(pattern_tone[target_keypad_index] / 12);
  } else {
    if (menu_semitones_active) {
      // do nothing for now
    } else if (menu_mode_active) {
      switch (key_num) {
        case 0: // Play mode reverse
          incrementor = -1;
          playback_mode = 0;
        break;
        case 1: // Play mode bounce
          playback_mode = 1;
        break;
        case 2: // Play mode forward
          incrementor = 1;
          playback_mode = 2;
        break;
        case 3: // Toggle trigger mode
          trigger_mode = !trigger_mode;
        break;
        case 4: // Pointer count 1
          pointers = 1;
        break;
        case 5: // Pointer count 2
          pointers = 2;
        break;
        case 6: // Pointer count 4
          pointers = 4;
        break;
        case 8: // Toggle wait for sync mode
          if (hold_for_sync) {
            hold_for_sync = false;
            holding_for_sync = false;
          } else {
            hold_for_sync = true;
            holding_for_sync = paused;
          }
        break;
        case 9: // Toggle loop pattern
          loop_pattern = !loop_pattern;
        break;
        case 10: // Toggle internal clock
          self_clock = !self_clock;
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

uint32_t tone_color(int semitone) {
  double redval = semitone + 5;
  double greenval = redval - 40 > 0 ? redval - 40 : 0;
  int blueval = 0;
  uint32_t returnval = trellis.pixels.Color(redval, greenval, blueval);
  return returnval;
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
      uint32_t pixelcolor = 0x000001;
      switch (i) {
        case 0:  // Reverse play direction
          pixelcolor = (playback_mode == 0) ? 0x991111 : 0x690101;
        break;
        case 1:  // Bounce play directions
          pixelcolor = (playback_mode == 1) ? 0x119911 : 0x016901;
        break;
        case 2:  // Forward play direction
          pixelcolor = (playback_mode == 2) ? 0x111199 : 0x010169;
        break;
        case 3:  // Toggle triggers only mode
          pixelcolor = trigger_mode ? 0x555500 : 0x252500;
        break;
        case 4:  // One play head
          pixelcolor = (pointers == 1) ? 0x999911 : 0x696901;
        break;
        case 5:  // Two play heads
          pixelcolor = (pointers == 2) ? 0x991199 : 0x690169;
        break;
        case 6:  // Four play heads
          pixelcolor = (pointers == 4) ? 0x119999 : 0x016969;
        break;
        case 8: // Toggle wait for sync mode
          pixelcolor = hold_for_sync ? 0x339933 : 0x135913;
        break;
        case 9: // Toggle loop pattern
          pixelcolor = loop_pattern ? 0x225566 : 0x021526;
        break;
        case 10: // Toggle auto clocking
          pixelcolor = self_clock ? 0x123566 : 0x020546;
        break;
      }
      trellis.pixels.setPixelColor(i, pixelcolor);
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
    uint32_t inactive_color = 0x000001;
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      if (i < 12) { // Semitones in top three rows of keypad
        int relative_semitone = (menu_semitones_octave * 12) + i;
        if (pattern_tone[target_keypad_index] == relative_semitone) {
          trellis.pixels.setPixelColor(i, tone_color(relative_semitone));
        } else if (i == 0 || i == 2 || i == 4 || i == 5 || i == 7 || i == 9 || i == 11) {
          trellis.pixels.setPixelColor(i, major_palette[menu_semitones_octave]);
        } else {
          trellis.pixels.setPixelColor(i, minor_palette[menu_semitones_octave]);
        }
      } else { // Octaves in bottom row of keypad
        if (menu_semitones_octave > 0 && (i - 11) == menu_semitones_octave) {
          trellis.pixels.setPixelColor(i, major_palette[menu_semitones_octave]);
        } else {
          trellis.pixels.setPixelColor(i, inactive_color);
        }
      }
    }
  } else {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      int pattern_index = (current_page * grid_size) + i;
      uint32_t active_color = is_pointer(i) ? 0xAAAAAA : keypad_color(i);
      uint32_t recording_color = 0xff0000;
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
          if (holding_for_sync && !blinker) {
            trellis.pixels.setPixelColor(i, (trigger_mode ? trigger_color : cv_color));
          } else if (recording_cv && blinker) {
            trellis.pixels.setPixelColor(i, recording_color);
          } else {
            trellis.pixels.setPixelColor(i, active_color);
          }
        } else if (trigger_mode) {
          trellis.pixels.setPixelColor(i, trigger_color);
        } else {
          trellis.pixels.setPixelColor(i, cv_color);
        }
      } else {
        if (keypads_down[i]) {
          trellis.pixels.setPixelColor(i, cv_color);
        } else if (is_pointer(i)) {
          if (holding_for_sync && !blinker) {
            trellis.pixels.setPixelColor(i, inactive_color);
          } else if (recording_cv && blinker) {
            trellis.pixels.setPixelColor(i, recording_color);
          } else {
            trellis.pixels.setPixelColor(i, pointer_color);
          }
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
