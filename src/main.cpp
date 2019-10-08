#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include "Adafruit_NeoTrellis.h"

#include <easing.h>
#include <pins.h>
#include <init.h>
#include <utils.h>
#include <sequencer.h>
#include <keypad.h>
#include <encoders.h>
#include <fram.h>
#include <dac.h>

#include <ui.h>

void setup() {
  setup_pins();

  Serial.begin(9600);
  SPI.begin();
  Wire.begin();
  initialise_mcp();
  initialise_trellis();
  initialise_fram();
  initialise_dac();
}

void loop() {
  if (!semitones[5]) {
    build_semitone_scale();
  }
  looptime = millis();
  update_timers();
  poll_clock();
  poll_rst();
  poll_btns();
  poll_encoders();
  sync_keypad();
  resolve_interactions();
  resolve_dacs();
}
