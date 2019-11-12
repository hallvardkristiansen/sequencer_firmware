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
  Serial.begin(9600);

  setup_pins();
  build_semitone_scale();

  SPI.begin();
  initialise_fram();
  initialise_dac();

  Wire.begin();
  initialise_mcp();
  initialise_trellis();

  read_fram();
}

void loop() {
  update_timers();
  poll_clock();
  poll_rst();
  poll_btns();
  poll_encoders();
  sync_keypad();
  resolve_interactions();
  resolve_dacs();
}
