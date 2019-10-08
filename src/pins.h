
// I2C IO Exp / Trellis
// SPI F-RAM / DAC

// Board:
const int clock_pin = 0;
const int rst_pin = 1; // connected to sync_out_pin over jack switch
const int dac_cs_pin = 3;
const int fram_cs_pin = 9;
const int ioexp_int_pin = 7;
const int trellis_int_pin = 2;
const int btn_dur_pin = 10;
const int btn_swing_pin = 11;
const int btn_steps_pin = 12;
const int btn_mode_pin = 13;
const int sync_out_pin = A0; // connected to rst_pin over jack switch
const int all_out_pin = A1;
const int cv_mode_pin = A2;
const int cv_steps_pin = A3;
const int cv_swing_pin = A4;
const int cv_dur_pin = A5;

// IO Expander:
const int io_steps_b_pin = 0;
const int io_steps_a_pin = 1;
const int io_mode_a_pin = 2;
const int io_mode_b_pin = 3;
const int io_dur_a_pin = 4;
const int io_dur_b_pin = 5;
const int io_swing_b_pin = 6;
const int io_swing_a_pin = 7;

void setup_pins() {
  pinMode(clock_pin, INPUT_PULLUP);
  pinMode(rst_pin, INPUT_PULLUP);
  pinMode(dac_cs_pin, OUTPUT);
  pinMode(fram_cs_pin, OUTPUT);
  pinMode(ioexp_int_pin, INPUT_PULLUP);
  pinMode(trellis_int_pin, INPUT_PULLUP);
  pinMode(btn_mode_pin, INPUT_PULLUP);
  pinMode(btn_steps_pin, INPUT_PULLUP);
  pinMode(btn_swing_pin, INPUT_PULLUP);
  pinMode(btn_dur_pin, INPUT_PULLUP);
  pinMode(cv_mode_pin, INPUT);
  pinMode(cv_steps_pin, INPUT);
  pinMode(cv_swing_pin, INPUT);
  pinMode(cv_dur_pin, INPUT);
}
