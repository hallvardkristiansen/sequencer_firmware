void initialise_fram() {
  digitalWrite(fram_cs_pin, HIGH);
}

void write_fram() {
  digitalWrite(fram_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(B00000110); // write enable
  SPI.transfer(B00000010); // begin write
  SPI.transfer(0x00); // address try 0x000000
  SPI.transfer(0x00); // address
  SPI.transfer(0x00); // address
  SPI.transfer(B10101110); // write byte
  SPI.transfer(B11111111); // write byte
  SPI.transfer(B11101010); // write byte
  SPI.transfer(B00111011); // write byte
  SPI.transfer(B01101011); // write byte
  SPI.endTransaction();
  digitalWrite(fram_cs_pin, HIGH);
}

void read_fram(int address, int length) {
  digitalWrite(fram_cs_pin, LOW);
  SPI.beginTransaction(spi_settings);
  SPI.transfer(B00000011); // begin read
  SPI.transfer(0x00); // address
  SPI.transfer(0x00); // address
  SPI.transfer(0x00); // address
  SPI.endTransaction();
  digitalWrite(fram_cs_pin, HIGH);
}

void test_fram() {
  write_fram();
  delay(1000);
  read_fram(B00000000, 5);
}
