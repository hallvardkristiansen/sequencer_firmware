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
        trellis.pixels.setPixelColor(i, 0x000001);
      }
    }
  }
}

void initialise_trellis() {
  trellis.begin();
  trellis_keypress_events();
  intro_animation();
}
