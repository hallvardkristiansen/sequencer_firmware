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

void semitone_to_dac(int dac, int note, int last_note) {
  uint16_t dac_value = semitones[note];
  if (global_glide > 0) {
    double time_elapsed = microtime - last_clock_time;
    double time_duration = glide_dur * global_glide;
    double time = time_elapsed / time_duration;
    if (time < 1.0) {
      double mult = 1.0;
      double note_diff = semitones[last_note] - semitones[note];
      switch(glide_mode) {
        case 0:
          mult = easeInOutSine(time);
        break;
        case 1:
          mult = easeInSine(time);
        break;
        case 2:
          mult = easeOutSine(time);
        break;
        case 3:
          mult = easeInOutQuad(time);
        break;
        case 4:
          mult = easeInQuad(time);
        break;
        case 5:
          mult = easeOutQuad(time);
        break;
        case 6:
          mult = easeInOutCubic(time);
        break;
        case 7:
          mult = easeInCubic(time);
        break;
        case 8:
          mult = easeOutCubic(time);
        break;
        case 9:
          mult = easeInOutQuart(time);
        break;
        case 10:
          mult = easeInQuart(time);
        break;
        case 11:
          mult = easeOutQuart(time);
        break;
        case 12:
          mult = easeInOutQuint(time);
        break;
        case 13:
          mult = easeInQuint(time);
        break;
        case 14:
          mult = easeOutQuint(time);
        break;
        case 15:
          mult = easeInOutExpo(time);
        break;
        case 16:
          mult = easeInExpo(time);
        break;
        case 17:
          mult = easeOutExpo(time);
        break;
        case 18:
          mult = easeInOutCirc(time);
        break;
        case 19:
          mult = easeInCirc(time);
        break;
        case 20:
          mult = easeOutCirc(time);
        break;
        case 21:
          mult = easeInOutBack(time);
        break;
        case 22:
          mult = easeInBack(time);
        break;
        case 23:
          mult = easeOutBack(time);
        break;
        case 24:
          mult = easeInOutElastic(time);
        break;
        case 25:
          mult = easeInElastic(time);
        break;
        case 26:
          mult = easeOutElastic(time);
        break;
        case 27:
          mult = easeInOutBounce(time);
        break;
        case 28:
          mult = easeInBounce(time);
        break;
        case 29:
          mult = easeOutBounce(time);
        break;
      }
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
        if (triggered && triggering) {
          set_dac(dacs_single[i], spi_dac_5v);
        } else {
          set_dac(dacs_single[i], spi_dac_0v);
        }
      } else {
        semitone_to_dac(i, notes[i], last_notes[i]);
      }
    }
    update_spi_dacs = false;
  }
  if (update_int_dacs) {
    if (syncing) {
      if (all_out) {
        analogWrite(all_out_pin, int_dac_5v);
      }
      if (sync_out) {
        analogWrite(sync_out_pin, int_dac_5v);
      }
    } else {
      analogWrite(all_out_pin, int_dac_0v);
      analogWrite(sync_out_pin, int_dac_0v);
      all_out = false;
      sync_out = false;
    }

    update_int_dacs = false;
  }
}

void initialise_dac() {
  digitalWrite(dac_cs_pin, HIGH);
  zero_dacs();
}
