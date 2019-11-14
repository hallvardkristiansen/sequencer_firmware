void initialise_fram() {
  digitalWrite(fram_cs_pin, HIGH);
}

void write_fram() {
  Serial.println("Saving...");
  spi_busy = true;
  digitalWrite(fram_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(B00000110); // write enable
  SPI.transfer(B00000010); // begin write
  SPI.transfer(0x00); // address
  SPI.transfer(notes_address); // address
  SPI.transfer(0x00); // address
  for (int i = 0; i < max_pattern_length; i++) {
    SPI.transfer(pattern_tone[i]);
    SPI.transfer(pattern_on[i]);
    SPI.transfer(pattern_swing[i]);
    SPI.transfer(pattern_glide[i]);
  }
  SPI.transfer(paused);
  SPI.transfer(hold_for_reset);
  SPI.transfer(pattern_length);
  SPI.transfer(current_page);
  SPI.transfer(glide_mode);
  SPI.transfer(global_glide);
  SPI.transfer(global_swing);
  SPI.endTransaction();
  digitalWrite(fram_cs_pin, HIGH);
  spi_busy = false;
}

void read_fram() {
  spi_busy = true;
  digitalWrite(fram_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(B00000011); // begin read
  SPI.transfer(0x00); // address
  SPI.transfer(notes_address); // address
  SPI.transfer(0x00); // address
  for (int i = 0; i < max_pattern_length; i++) {
    pattern_tone[i] = SPI.transfer(0x00);
    pattern_on[i] = SPI.transfer(0x00);
    pattern_swing[i] = SPI.transfer(0x00);
    pattern_glide[i] = SPI.transfer(0x00);
  }
  paused = SPI.transfer(0x00);
  hold_for_reset = SPI.transfer(0x00);
  pattern_length = SPI.transfer(0x00);
  current_page = SPI.transfer(0x00);
  glide_mode = SPI.transfer(0x00);
  global_glide = SPI.transfer(0x00);
  global_swing = SPI.transfer(0x00);
  SPI.endTransaction();
  digitalWrite(fram_cs_pin, HIGH);
  spi_busy = false;
}
