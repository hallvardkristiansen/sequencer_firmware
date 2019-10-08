byte dacs_single[] = {
  B00110010, // A
  B00110001, // B
  B00110100, // C
  B00111000  // D
};
byte dacs_all = B00111111;

void set_dac(byte dacs, uint16_t note) {
  byte note_msb = note >> 8;
  byte note_lsb = note & 0x00ff;
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  digitalWrite(dac_cs_pin, LOW);
  SPI.transfer(dacs);
  SPI.transfer(note_msb);
  SPI.transfer(note_lsb);
  digitalWrite(dac_cs_pin, HIGH);
}

void zero_dacs() {
  set_dac(dacs_all, spi_dac_0v);
  analogWrite(all_out_pin, int_dac_0v);
  analogWrite(sync_out_pin, int_dac_0v);
}

void build_semitone_scale() {
  uint16_t range = spi_dac_0v - spi_dac_5v;
  float step = (float)range / 60.0;
  for (int i = 0; i < 60; i++) {
    uint16_t tone = (float)spi_dac_0v - (step * (float)i);
    semitones[i] = tone;
  }
}

void semitone_to_dac(int dac, int note) {
  // add glide implementation here
  set_dac(dacs_single[dac], semitones[note]);
}

void resolve_dacs() {
  if (update_dacs) {
    for (int i = 0; i < 4; i++) {
      if (triggers[i]) {
        if (triggered) {
          set_dac(dacs_single[i], spi_dac_5v);
        } else {
          set_dac(dacs_single[i], spi_dac_0v);
        }
      } else {
        semitone_to_dac(i, notes[i]);
      }
    }

    if (all_out) {
      // Output all out signal whatever that may be later
      analogWrite(all_out_pin, int_dac_5v);
      all_out = false;
    } else {
      analogWrite(all_out_pin, int_dac_0v);
    }

    if (sync_out) {
      analogWrite(sync_out_pin, int_dac_5v);
      sync_out = false;
    } else {
      analogWrite(sync_out_pin, int_dac_0v);
    }

    update_dacs = false;
  }
}

void initialise_dac() {
  digitalWrite(dac_cs_pin, HIGH);
  zero_dacs();
}
