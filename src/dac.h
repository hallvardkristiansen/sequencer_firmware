byte dacs_single[] = {
  B00110010, // A
  B00110001, // B
  B00110100, // C
  B00111000  // D
};
byte dacs_all = B00111111;

void set_dac(byte dacs, uint16_t note) {
  spi_busy = true;
  byte note_msb = note >> 8;
  byte note_lsb = note & 0x00ff;
  digitalWrite(dac_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(dacs);
  SPI.transfer(note_msb);
  SPI.transfer(note_lsb);
  SPI.endTransaction();
  digitalWrite(dac_cs_pin, HIGH);
  spi_busy = false;
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

void semitone_to_dac(int dac, int note, int last_note, int key_glide) {
  uint16_t dac_value = semitones[note];
  if (global_glide > 0 || key_glide > 0) {
    double time_elapsed = microtime - last_clock_time;
    double time_duration = (glide_dur * global_glide) + (glide_dur * key_glide);
    double time = time_elapsed / time_duration;
    if (time < 0.0) {
      dac_value = semitones[last_note];
    } else if (time < 1.0) {
      double mult = get_easing(time);
      double note_diff = semitones[last_note] - semitones[note];
      double note_offset = note_diff * mult;
      dac_value = semitones[last_note] - (int)note_offset;
    }
  }
  set_dac(dacs_single[dac], dac_value);
}

void resolve_dacs() {
  if (update_spi_dacs && !spi_busy) {
    for (int i = 0; i < 4; i++) {
      if (triggers[i]) {
        if (triggering) {
          set_dac(dacs_single[i], spi_dac_5v);
        } else {
          set_dac(dacs_single[i], spi_dac_0v);
        }
      } else {
        semitone_to_dac(i, notes[i], last_notes[i], glide[i]);
      }
    }
  }
  if (update_int_dacs) {
    if (syncing) {
      analogWrite(sync_out_pin, int_dac_5v);
    } else {
      analogWrite(sync_out_pin, int_dac_0v);
    }
    if (triggering) {
      analogWrite(all_out_pin, int_dac_5v);
    } else {
      analogWrite(all_out_pin, int_dac_0v);
    }
  }
}

void initialise_dac() {
  digitalWrite(dac_cs_pin, HIGH);
  zero_dacs();
}
