void pause() {
  if (hold_for_sync) {
    holding_for_sync = true;
  } else {
    paused = true;
  }
}

void fire_sync() {
  last_sync_time = microtime;
  sync_primed = false;
}

void change_pointers_count(int dir) {
  if (dir > 0) {
    if (pointers == 1) {
      pointers = 2;
    } else if (pointers == 2) {
      pointers = 4;
    } else if (pointers == 4) {
      pointers = 1;
    }
  } else {
    if (pointers == 4) {
      pointers = 2;
    } else if (pointers == 2) {
      pointers = 1;
    } else if (pointers == 1) {
      pointers = 4;
    }
  }
}

void change_pattern_length(int amnt) {
  if (amnt > 0) {
    pattern_length += grid_size * amnt;
  } else {
    int deduct = grid_size * (amnt * -1);
    if (pattern_length - deduct < grid_size) {
      pattern_length = grid_size;
    } else {
      pattern_length -= deduct;
    }
  }
}

void change_pattern_start(int amnt) {
  if (amnt > 0) {
    pattern_start += grid_size * amnt;
  } else {
    int deduct = grid_size * (amnt * -1);
    if (pattern_start - deduct < 0) {
      pattern_start = 0;
    } else {
      pattern_start -= deduct;
    }
  }
  current_page = pattern_start / grid_size;
}

bool is_playing() {
  bool playing = hold_for_sync ? !holding_for_sync : !paused;
  return playing;
}

void reset_pointer() {
  if (playback_mode == 1) {
    incrementor = -incrementor;
    if (incrementor > 0) {
      pointer = 1;
      current_page = pattern_start / grid_size;
    } else {
      pointer = (grid_size / pointers) - 2;
      current_page = ((pattern_start + pattern_length) / grid_size) - 1;
    }
  } else {
    if (incrementor > 0) {
      pointer = 0;
      current_page = pattern_start / grid_size;
    } else {
      pointer = (grid_size / pointers) - 1;
      current_page = ((pattern_start + pattern_length) / grid_size) - 1;
    }
  }
  pattern_ended = false;
  sync_primed = true;
}

void update_pointer(int val) {
  int max_steps = grid_size / pointers;
  int pointer_min = 0;
  int pointer_max = max_steps - 1;
  bool pointer_over_bounds = pointer + val > pointer_max;
  bool pointer_under_bounds = pointer + val < pointer_min;
  bool page_over_bounds = (current_page + 1) * grid_size >= pattern_start + pattern_length;
  bool page_under_bounds = (current_page - 1) * grid_size < pattern_start;

  pattern_ended = (pointer_over_bounds && page_over_bounds) || (pointer_under_bounds && page_under_bounds);

  if (pattern_ended && is_playing()) {
    reset_pointer();
    if (!loop_pattern) {
      pause();
    }
  } else {
    if (pointer_over_bounds && !page_over_bounds) {
      pointer = pointer_min;
      current_page++;
    } else if (pointer_under_bounds && !page_under_bounds) {
      pointer = pointer_max;
      current_page--;
    } else {
      if ((pointer == pointer_min && sync_primed) || (pointer == pointer_max && sync_primed)) {
        fire_sync();
      }

      pointer += val;
    }
  }
}

bool is_pointer(int val) {
  bool returnval = false;
  int steplength = (gridx * gridy) / pointers;
  for (int i = 0; i < pointers; i++) {
    if (val == pointer + (steplength * i)) {
      returnval = true;
    }
  }
  return returnval;
}

void increment_sequence() {
  int steplength = grid_size / pointers;
  pattern_pointer = (current_page * grid_size) + pointer;
  swinging = pointer % 2 == 0;
  for (int i = 0; i < dac_count; i++) {
    last_notes[i] = notes[i];
    last_swing[i] = swing[i];
    last_glide[i] = glide[i];
  }
  for (int i = 0; i < pointers; i++) {
    int this_pointer = pattern_pointer + (steplength * i);
    int this_note = last_notes[i];
    int this_swing = last_swing[i];
    int this_glide = last_glide[i];
    bool set_triggers = false;
    if (pattern_on[this_pointer]) {
      this_note = pattern_tone[this_pointer];
      this_swing = pattern_swing[this_pointer];
      this_glide = pattern_glide[this_pointer];
      set_triggers = true;
    }
    if (pointers == 1) {
      if (trigger_mode) {
        triggers[i] = set_triggers;
      } else {
        notes[0] = this_note;
        swing[0] = this_swing;
        glide[0] = this_glide;
        triggers[0] = false;
        triggers[1] = set_triggers;
        notes[2] = this_note;
        swing[2] = this_swing;
        glide[2] = this_glide;
        triggers[2] = false;
        triggers[3] = set_triggers;
      }
    }
    if (pointers == 2) {
      if (trigger_mode) {
        triggers[i] = set_triggers;
      } else {
        if (i == 0) {
          notes[0] = this_note;
          swing[0] = this_swing;
          glide[0] = this_glide;
          triggers[0] = false;
          triggers[1] = set_triggers;
        }
        if (i == 1) {
          notes[2] = this_note;
          swing[2] = this_swing;
          glide[2] = this_glide;
          triggers[2] = false;
          triggers[3] = set_triggers;
        }
      }
    }
    if (pointers == 4) {
      if (trigger_mode) {
        triggers[i] = set_triggers;
      } else {
        notes[i] = this_note;
        swing[i] = this_swing;
        glide[i] = this_glide;
      }
    }
  }
  refresh_trellis = true;
}

void increment_octave(int amnt) {
  if (menu_semitones_octave + amnt < 5 && menu_semitones_octave + amnt >= 0) {
    menu_semitones_octave += amnt;
  }
}
void increment_note(int amnt) {
  if (pattern_tone[last_keypad_down_index] + amnt >= (int)sizeof(semitones)) {
    pattern_tone[last_keypad_down_index] = (int)sizeof(semitones) -1;
  } else if (pattern_tone[last_keypad_down_index] + amnt < 0) {
    pattern_tone[last_keypad_down_index] = 0;
  } else {
    pattern_tone[last_keypad_down_index] += amnt;
  }
}
void increment_key_swing(int amnt) {
  for (int i = 0; i < (int)sizeof(keypads_down); i++) {
    if (keypads_down[i]) {
      if (pattern_swing[i] + amnt > 255) {
        pattern_swing[i] = 255;
      } else if (pattern_swing[i] + amnt < 0) {
        pattern_swing[i] = 0;
      } else {
        pattern_swing[i] += amnt;
      }
    }
  }
}
void increment_swing(int amnt) {
  if (global_swing + amnt > 255) {
    global_swing = 255;
  } else if (global_swing + amnt < 0) {
    global_swing = 0;
  } else {
    global_swing += amnt;
  }
}
void increment_key_glide(int amnt) {
  for (int i = 0; i < (int)sizeof(keypads_down); i++) {
    if (keypads_down[i]) {
      if (pattern_glide[i] + amnt > 255) {
        pattern_glide[i] = 255;
      } else if (pattern_glide[i] + amnt < 0) {
        pattern_glide[i] = 0;
      } else {
        pattern_glide[i] += amnt;
      }
    }
  }
}
void increment_glide(int amnt) {
  if (global_glide + amnt > 255) {
    global_glide = 255;
  } else if (global_glide + amnt < 0) {
    global_glide = 0;
  } else {
    global_glide += amnt;
  }
}
void increment_glide_mode(int amnt) {
  if (glide_mode + amnt > glide_modes) {
    glide_mode = glide_modes;
  } else if (glide_mode + amnt < 0) {
    glide_mode = 0;
  } else {
    glide_mode += amnt;
  }
}

void fire_reset() {
  if (!loop_pattern) {
    reset_pointer();
  }
  if (hold_for_sync) {
    holding_for_sync = false;
    paused = false;
  }
}

void fire_trigger() {
  blinker = !blinker;
  clock_interval = microtime - last_clock_time;
  last_clock_time = microtime;
  if (is_playing()) {
    swing_delay = swinging ? global_swing * swing_dur : 0;
    increment_sequence();
    update_pointer(incrementor);
    if (recording_cv) {
      sample_cv = true;
    }
  }
  refresh_trellis = true;
}

void change_page(int dir) {
  int upcoming_index = (current_page + dir) * grid_size;
  bool page_over_bounds = upcoming_index >= pattern_start + pattern_length;
  bool page_under_bounds = upcoming_index < pattern_start;
  if (!page_over_bounds && !page_under_bounds) {
    current_page += dir;
  }
}

void copy_page() {
  copy_section[0] = current_page * grid_size;
  copy_section[1] = grid_size;
}

void paste_page() {
  int starting_step = current_page * grid_size;
  for (int i = 0; i < copy_section[1]; i++) {
    int paste_step = starting_step + i;
    int copy_step = copy_section[0] + i;
    pattern_tone[paste_step] = pattern_tone[copy_step];
    pattern_swing[paste_step] = pattern_swing[copy_step];
    pattern_glide[paste_step] = pattern_glide[copy_step];
    pattern_on[paste_step] = pattern_on[copy_step];
  }
  copy_section[1] = 0;
}

void fill_active_pattern() {
  for (int i = 0; i < pattern_length; i++) {
    int paste_step = pattern_start + i;
    int copy_position = i % grid_size;
    int copy_step = copy_section[0] + copy_position;
    pattern_tone[paste_step] = pattern_tone[copy_step];
    pattern_swing[paste_step] = pattern_swing[copy_step];
    pattern_glide[paste_step] = pattern_glide[copy_step];
    pattern_on[paste_step] = pattern_on[copy_step];
  }
  copy_section[1] = 0;
}

void fill_nth_page(int page_num) {
  int page = current_page;
  for (int i = (current_page * grid_size) - 1; i < pattern_length; i++) {
    if (page % page_num == 0) {
      int paste_step = pattern_start + i;
      int copy_position = i % grid_size;
      int copy_step = copy_section[0] + copy_position;
      pattern_tone[paste_step] = pattern_tone[copy_step];
      pattern_swing[paste_step] = pattern_swing[copy_step];
      pattern_glide[paste_step] = pattern_glide[copy_step];
      pattern_on[paste_step] = pattern_on[copy_step];
    }
    if (i % grid_size == 0) {
      page++;
    }
  }
  copy_section[1] = 0;
}

void clear_page() {
  int starting_step = current_page * grid_size;
  for (int i = 0; i < grid_size; i++) {
    int clear_step = starting_step + i;
    pattern_tone[clear_step] = 0;
    pattern_swing[clear_step] = 0;
    pattern_glide[clear_step] = 0;
    pattern_on[clear_step] = false;
  }
}

void randomize_page() {
  int starting_step = current_page * grid_size;
  for (int i = 0; i < grid_size; i++) {
    int this_step = starting_step + i;
    int random_integer = 0 + rand() % 60;
    pattern_tone[this_step] = random_integer;
    pattern_on[this_step] = random_integer > 30;
  }
}

int get_swing() {
  return pattern_swing[last_keypad_down_index] > 0 ? pattern_swing[last_keypad_down_index] : global_swing;
}

int get_glide() {
  return pattern_glide[last_keypad_down_index] > 0 ? pattern_glide[last_keypad_down_index] : global_glide;
}
