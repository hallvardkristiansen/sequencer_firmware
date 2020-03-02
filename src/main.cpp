#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <time.h>
#include "Adafruit_NeoTrellis.h"

#include <pins.h>
#include <init.h>
#include <fram.h>
#include <easing.h>
#include <dac.h>
#include <encoders.h>

#include <sequencer.h>
#include <ui.h>
#include <polling.h>
#include <utils.h>

void setup() {
  //Serial.begin(9600);

  setup_pins();
  build_semitone_scale();

  SPI.begin();
  initialise_fram();
  initialise_dac();

  Wire.begin();
  Wire.setClock(i2c_hz);

  initialise_mcp();
  initialise_trellis();

  read_fram();
}

void loop() {
  poll_inputs();
  update_timers();
  resolve_dacs();
  poll_ui();
  resolve_interactions();
  save_state();
  refresh_keypad();
  //serial_print_debug();
}
