Adafruit_NeoTrellis trellis;
int io_expander_addr, keypad_addr;

SPISettings spi_settings(24000000, MSBFIRST, SPI_MODE0);
const int i2c_hz = 400000;
const uint16_t spi_dac_0v = 0xa100;
const uint16_t spi_dac_5v = 0x1e00;
const uint16_t int_dac_0v = 0x076c;
const uint16_t int_dac_5v = 0x00c8;
const uint16_t int_dac_range = 0x0fa0;

const int debug_serial_hz = 2000; // millis
const int btn_poll_hz = 7; // millis
const int key_poll_hz = 61; // millis
const int save_hz = 10007; // millis
const int btn_hold_wait = 500; // millis
const int temp_menu_dur = 2000; // millis

const int spi_dac_hz = 191; // micros
const int int_dac_hz = 997; // micros
const int int_adc_hz = 1493; // micros
const int trigger_dur = 15000; // micros
const int sync_dur = 20000; // micros
const int swing_dur = 1000; // micros
const int glide_dur = 5000; // micros

const uint32_t major_palette[5] = {0x550000, 0x330033, 0x000055, 0x003333, 0x005500};
const uint32_t minor_palette[5] = {0x150000, 0x050005, 0x000015, 0x000505, 0x001500};

bool swinging = false;
int swing_delay = 0;
int glide_mode = 0;
const int glide_modes = 29;

bool adc_poll = false;
bool sample_cv = false;
bool recording_cv = false;
int cv_mode = 0;
int cv_steps = 0;
int cv_swing = 0;
int cv_dur = 0;

long millitime = 0; // millis
long microtime = 0; // micros
long trellis_delay = 0; // micros
long last_key_polltime = 0; // millis
long last_btn_polltime = 0; // millis
long last_btn_press = 0; // millis
long last_int_dac_update = 0; // micros
long last_int_adc_update = 0; // micros
long last_spi_dac_update = 0; // micros
long last_clock_time = 0; // micros
long last_sync_time = 0; // micros
long last_save_time = 0; // millis
long last_print_time = 0; // millis
long last_enc_action = 0; // millis

const int gridx = 4;
const int gridy = 4;
const int grid_size = 16;
const int initial_pattern_length = 32;
const int max_pattern_length = 2048;
const int dac_count = 4;
const int trellis_led_delay = 30; // microsecond delay per LED
int current_page = 0;
int pointer = 0;
int pointers = 1;
int playback_mode = 0;
int incrementor = 1;

int pattern_length = initial_pattern_length;
int pattern_start = 0;
int pattern_pointer = 0;
int pattern_tone[max_pattern_length] {30};
int pattern_swing[max_pattern_length] {0};
int pattern_glide[max_pattern_length] {0};
bool pattern_on[max_pattern_length] {false};
int copy_section[2] {0};
int global_swing = 0;
int global_glide = 0;
byte setting_ui[grid_size] {0x00};
bool triggers[dac_count] {false};
int notes[dac_count] {0};
int last_notes[dac_count] {0};
int glide[dac_count] {0};
int last_glide[dac_count] {0};
int swing[dac_count] {0};
int last_swing[dac_count] {0};
bool update_spi_dacs = true;
bool update_int_dacs = true;
bool keypad_down = false;
bool keypads_down[grid_size] {false};
int last_keypad_down_index = 0;
int target_keypad_index = 0;
uint16_t semitones[60];

bool print_debug = false;
bool paused = false;
bool triggered = false;
bool triggering = true;
bool forward_clock = false;
bool reset = false;
bool pattern_ended = false;
bool syncing = false;
bool sync_primed = true;
bool holding_for_sync = false;
bool hold_for_sync = false;
bool loop_pattern = true;
bool polling_keys = false;
bool polling_btns = false;
bool apply_modifiers = false;
bool spi_busy = false;
bool refresh_trellis = true;
bool prime_keypad_refresh = false;
bool perform_save = false;
bool blinker = false;
bool trigger_mode = false;

bool btn_mode_state = false;
bool btn_steps_state = false;
bool btn_swing_state = false;
bool btn_dur_state = false;
bool btn_mode_down = false;
bool btn_steps_down = false;
bool btn_swing_down = false;
bool btn_dur_down = false;
bool btn_hold_primed = false;
bool all_btns_pressed = false;
bool keypad_mode_menu = false;
bool enc_modified = false;
int enc_mode_mod = 0;
int enc_steps_mod = 0;
int enc_swing_mod = 0;
int enc_dur_mod = 0;

bool menu_mode_active = false;
bool menu_steps_active = false;
bool menu_swing_active = false;
bool menu_dur_active = false;
bool menu_semitones_active = false;
int menu_semitones_octave = 0;
