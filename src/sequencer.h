void update_pointer(int val) {
  int max_steps = grid_size / pointers;
  int pointer_min = 0;
  int pointer_max = max_steps - 1;
  if (pointer + 1 == pointer_max && (current_page + 1) * grid_size == pattern_length) {
    sync_out = true;
  }
  if (pointer + val > pointer_max) {
    if (((current_page + 1) * grid_size) < pattern_length) {
      pointer = pointer_min;
      current_page++;
    }
  } else if (pointer + val < pointer_min) {
    if (((current_page - 1) * grid_size) >= 0) {
      pointer = pointer_max;
      current_page--;
    }
  } else {
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

void increment_sequence(int val) {
  int steplength = grid_size / pointers;
  if (paused && val < 0) {
    reset_on_increment = false;
  }
  if (reset_on_increment) {
    pointer = 0;
    current_page = 0;
    val = 0;
    reset_on_increment = false;
  }
  update_pointer(val);
  pattern_pointer = (current_page * grid_size) + pointer;
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
      if (paused) {
        last_clock_time = microtime;
        triggered = true;
        all_out = true;
        swinging = !swinging;
      }
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

void increment_note(int amnt) {
  for (int i = 0; i < (int)sizeof(keypads_down); i++) {
    if (keypads_down[i]) {
      int tone_index = (current_page * grid_size) + i;
      if (pattern_tone[tone_index] + amnt >= (int)sizeof(semitones)) {
        pattern_tone[tone_index] = (int)sizeof(semitones) -1;
      } else if (pattern_tone[tone_index] + amnt < 0) {
        pattern_tone[tone_index] = 0;
      } else {
        pattern_tone[tone_index] += amnt;
      }
    }
  }
  increment_sequence(0);
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
    Serial.println(global_swing);
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
    Serial.println(global_glide);
  }
}
void increment_glide_mode(int amnt) {
  if (glide_mode + amnt > glide_modes) {
    glide_mode = glide_modes;
  } else if (glide_mode + amnt < 0) {
    glide_mode = 0;
  } else {
    glide_mode += amnt;
    Serial.println(glide_mode);
  }
}

void fire_trigger() {
  triggered = true;
  last_clock_time = microtime;
  all_out = true;
  swinging = !swinging;
}

void fire_reset() {
  reset = true;
  reset_on_increment = true;
}
