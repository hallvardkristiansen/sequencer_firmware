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

void update_pointer(int val) {
  int max_steps = grid_size / pointers;
  int pointer_min = 0;
  int pointer_max = max_steps - 1;
  bool pointer_over_bounds = pointer + val > pointer_max;
  bool pointer_under_bounds = pointer + val < pointer_min;
  bool page_over_bounds = (current_page + 1) * grid_size >= pattern_start + pattern_length;
  bool page_under_bounds = (current_page - 1) * grid_size < pattern_start;

  pattern_ended = (pointer_over_bounds && page_over_bounds) || (pointer_under_bounds && page_under_bounds);

  if (pointer_over_bounds && !page_over_bounds) {
    pointer = pointer_min;
    current_page++;
  } else if (pointer_under_bounds && !page_under_bounds) {
    pointer = pointer_max;
    current_page--;
  } else if (!pattern_ended) {
    pointer += val;
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
  }
  for (int i = 0; i < pointers; i++) {
    int this_pointer = pattern_pointer + (steplength * i);
    int this_note = 0;
    bool set_triggers = false;
    if (pattern_on[this_pointer]) {
      this_note = pattern_tone[this_pointer];
      set_triggers = true;
    }
    if (pointers == 1) {
      notes[0] = this_note;
      triggers[0] = false;
      triggers[1] = set_triggers;
      notes[2] = this_note;
      triggers[2] = false;
      triggers[3] = set_triggers;
    }
    if (pointers == 2) {
      if (i == 0) {
        notes[0] = this_note;
        triggers[0] = false;
        triggers[1] = set_triggers;
      }
      if (i == 1) {
        notes[2] = this_note;
        triggers[2] = false;
        triggers[3] = set_triggers;
      }
    }
    if (pointers == 4) {
      triggers[i] = set_triggers;
    }
  }
  refresh_trellis = true;
}

void increment_note(int amnt) {
  if (pattern_tone[last_keypad_down_index] + amnt >= (int)sizeof(semitones)) {
    pattern_tone[last_keypad_down_index] = (int)sizeof(semitones) -1;
  } else if (pattern_tone[last_keypad_down_index] + amnt < 0) {
    pattern_tone[last_keypad_down_index] = 0;
  } else {
    pattern_tone[last_keypad_down_index] += amnt;
  }
  last_clock_time = microtime;
  increment_sequence();
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

void fire_trigger() {
  if (!paused && !pattern_ended) {
    update_pointer(incrementor);
    last_clock_time = swinging ? microtime + (global_swing * swing_dur) : microtime;
    if (!pattern_ended) {
      increment_sequence();
    } else {
      last_sync_time = microtime;
    }
  }
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
}

void fire_reset() {
  reset_pointer();
  increment_sequence();
}
