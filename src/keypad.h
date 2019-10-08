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

bool is_pointer(int val) {
  bool returnval = false;
  int steplength = (gridx * gridy) / pointers;
  for (int i = 0; i < pointers; i++) {
    if (val == pointer + (steplength * i)) {
      returnval = true;
    }
  }
  return returnval;
}

TrellisCallback keypress(keyEvent evt){
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    keypads_down[evt.bit.NUM] = true;
    keypad_down = true;
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    keypads_down[evt.bit.NUM] = false;
    keypad_down = false;
    int pattern_index = (grid_size * current_page) + evt.bit.NUM;
    if (!pattern_on[pattern_index]) {
      pattern_on[pattern_index] = true;
    } else if (!enc_modified && pattern_on[pattern_index]) {
      pattern_on[pattern_index] = false;
    }
    enc_modified = false;
  }
  increment_sequence(0);
  return 0;
}

void trellis_keypress_events() {
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, keypress);
  }
}

void intro_animation() {
  for (int j=0; j<=2; j++) {
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      trellis.pixels.setPixelColor(i, 0x901000);
      trellis.pixels.show();
      delay(5);
    }
    for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
      trellis.pixels.setPixelColor(i, 0x090100);
      trellis.pixels.show();
      delay(5);
    }
  }
}

uint32_t keypad_color(int num) {
  double mult = 255 / sizeof(semitones);
  double val = pattern_tone[num] * mult;
  uint32_t returnval = Wheel((int)val);
  return returnval;
}

void refresh_keypad_colours() {
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
        trellis.pixels.setPixelColor(i, 0x090100);
      }
    }
  }
}

void initialise_trellis() {
  trellis.begin();
  trellis_keypress_events();
  intro_animation();
}
