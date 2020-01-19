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
    int this_swing = 0;
    int this_glide = 0;
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
    last_clock_time = microtime;
    swing_delay = swinging ? global_swing * swing_dur : 0;
    if (!pattern_ended) {
      increment_sequence();
    } else if (sync_primed) {
      last_sync_time = microtime;
      sync_primed = false;
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
  sync_primed = true;
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

void insert_spaces() {
  int starting_step = current_page * grid_size;
  int orig_tone[grid_size] {0};
  int orig_swing[grid_size] {0};
  int orig_glide[grid_size] {0};
  bool orig_on[grid_size] {false};
  for (int i = 0; i < grid_size; i++) {
    int this_step = starting_step + i;
    orig_tone[i] = pattern_tone[this_step];
    orig_swing[i] = pattern_swing[this_step];
    orig_glide[i] = pattern_glide[this_step];
    orig_on[i] = pattern_on[this_step];
  }
  int j = 0;
  bool insert_blank = false;
  for (int i = 0; i < grid_size; i++) {
    if (j < grid_size) {
      int this_step = starting_step + i + j;
      if (insert_blank) {
        pattern_tone[this_step] = 0;
        pattern_swing[this_step] = 0;
        pattern_glide[this_step] = 0;
        pattern_on[this_step] = false;
        j++;
        insert_blank = false;
      } else {
        pattern_tone[this_step] = orig_tone[i];
        pattern_swing[this_step] = orig_swing[i];
        pattern_glide[this_step] = orig_glide[i];
        pattern_on[this_step] = orig_on[i];
      }
      if (orig_on[i]) {
        insert_blank = true;
      }
    }
  }
}

void remove_spaces() {
  int starting_step = current_page * grid_size;
  int orig_tone[grid_size] {0};
  int orig_swing[grid_size] {0};
  int orig_glide[grid_size] {0};
  bool orig_on[grid_size] {false};
  for (int i = 0; i < grid_size; i++) {
    int this_step = starting_step + i;
    orig_tone[i] = pattern_tone[this_step];
    orig_swing[i] = pattern_swing[this_step];
    orig_glide[i] = pattern_glide[this_step];
    orig_on[i] = pattern_on[this_step];
  }
  int j = 0;
  for (int i = 0; i < grid_size; i++) {
    int this_step = starting_step + i;
    if (i % 2 != 0) {
      j = i * 2;
      if (j < grid_size) {
        pattern_tone[this_step] = orig_tone[j];
        pattern_swing[this_step] = orig_swing[j];
        pattern_glide[this_step] = orig_glide[j];
        pattern_on[this_step] = orig_on[j];
      } else {
        pattern_tone[this_step] = 0;
        pattern_swing[this_step] = 0;
        pattern_glide[this_step] = 0;
        pattern_on[this_step] = false;
      }
    }
  }
}

int get_swing() {
  return pattern_swing[last_keypad_down_index] > 0 ? pattern_swing[last_keypad_down_index] : global_swing;
}

int get_glide() {
  return pattern_glide[last_keypad_down_index] > 0 ? pattern_glide[last_keypad_down_index] : global_glide;
}
