void initialise_fram() {
  digitalWrite(fram_cs_pin, HIGH);
}

void write_fram() {
  digitalWrite(fram_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(B00000110); // write enable
  SPI.transfer(B00000010); // begin write
  SPI.transfer(0x00); // address
  SPI.transfer(notes_address); // address
  SPI.transfer(0x00); // address
  for (int i = 0; i < max_pattern_length; i++) {
    SPI.transfer(pattern_tone[i]);
  }
  SPI.endTransaction();
  digitalWrite(fram_cs_pin, HIGH);
}

void read_fram() {
  digitalWrite(fram_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(B00000011); // begin read
  SPI.transfer(0x00); // address
  SPI.transfer(notes_address); // address
  SPI.transfer(0x00); // address
  for (int i = 0; i < max_pattern_length; i++) {
    pattern_tone[i] = SPI.transfer(0x00);
  }
  SPI.endTransaction();
  digitalWrite(fram_cs_pin, HIGH);
}
