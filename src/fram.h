void initialise_fram() {
  digitalWrite(fram_cs_pin, HIGH);
}

void test_fram() {
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  digitalWrite(fram_cs_pin, LOW);
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
  digitalWrite(fram_cs_pin, HIGH);
  delay(1000);
  digitalWrite(fram_cs_pin, LOW);
  SPI.transfer(B00000011); // begin read
  SPI.transfer(B00000000); // address
  SPI.transfer(B00000000); // address
  SPI.transfer(B00000000); // address
  byte result1, result2, result3, result4, result5;
  result1 = SPI.transfer(0x00);
  result2 = SPI.transfer(0x00);
  result3 = SPI.transfer(0x00);
  result4 = SPI.transfer(0x00);
  result5 = SPI.transfer(0x00);
  digitalWrite(fram_cs_pin, HIGH);
  Serial.println(result1);
  Serial.println(result2);
  Serial.println(result3);
  Serial.println(result4);
  Serial.println(result5);
}
