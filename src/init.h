Adafruit_NeoTrellis trellis;
int io_expander_addr, keypad_addr;

SPISettings spi_settings(24000000, MSBFIRST, SPI_MODE0);
const uint16_t spi_dac_0v = 0xa100;
const uint16_t spi_dac_5v = 0x1e00;
const uint16_t int_dac_0v = 0x076c;
const uint16_t int_dac_5v = 0x00c8;
const uint16_t int_dac_range = 0x0fa0;

const unsigned long poll_hz = 20; // millis
const unsigned long dac_hz = 1000; // micros
const unsigned long temp_menu_dur = 2000; // millis
const unsigned long trigger_dur = 10000; // micros
unsigned long mod_dur = 100000; // micros

long swing_amnt = 0;
long glide_amnt = 0;
int glide_mode = 0;
int glide_modes = 5;

unsigned long looptime = 0;
unsigned long signaltime = 0;
unsigned long last_looptime = 0;
unsigned long last_signaltime = 0;
unsigned long last_clock_time = 0;
unsigned long last_sync_time = 0;

const int gridx = 4;
const int gridy = 4;
const int grid_size = 16;
const int initial_pattern_length = 64;
int current_page = 0;
int current_x = 0;
int current_y = 0;
int pointer = 0;
int pointers = 1;
long pattern_length = initial_pattern_length;
long pattern_pointer = 0;
int pattern_tone[initial_pattern_length] {0};
int pattern_swing[initial_pattern_length] {0};
int pattern_glide[initial_pattern_length] {0};
bool pattern_on[initial_pattern_length] {false};
int global_swing = 0;
int global_glide = 0;
byte setting_ui[grid_size] {0x00};
bool triggers[4] {false};
int notes[4] {0};
int last_notes[4] {0}; 
bool update_spi_dacs = true;
bool update_int_dacs = true;
bool keypad_down = false;
bool keypads_down[grid_size] {false};
uint16_t semitones[60];

bool paused = false;
bool triggered = false;
bool triggering = true;
bool reset = false;
bool hold_for_reset = false;
bool all_out = false;
bool sync_out = false;
bool polling = false;
bool apply_modifiers = false;

bool btn_mode_down = false;
bool btn_steps_down = false;
bool btn_swing_down = false;
bool btn_dur_down = false;
bool enc_modified = false;
int enc_mode_mod = 0;
int enc_steps_mod = 0;
int enc_swing_mod = 0;
int enc_dur_mod = 0;
