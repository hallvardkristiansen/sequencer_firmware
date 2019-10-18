void update_pointer(int val) {
  int max_steps = grid_size / pointers;
  int pointer_min = 0;
  int pointer_max = max_steps - 1;
  if (pointer + val > pointer_max) {
    if (((current_page + 1) * grid_size) < pattern_length) {
      pointer = pointer_min;
      current_page++;
    } else {
      sync_out = true;
      hold_for_reset = true;
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

void increment_sequence(int val) {
  int steplength = grid_size / pointers;
  if (val == 99) {
    pointer = 0;
    current_page = 0;
    val = 0;
    hold_for_reset = false;
  }
  if (!hold_for_reset) {
    update_pointer(val);
    pattern_pointer = (current_page * grid_size) + pointer;
    last_notes = notes;
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
      if (pattern_swing[i] + amnt > (int)sizeof(semitones)) {
        pattern_swing[i] = 0;
      } else if (pattern_swing[i] + amnt < 0) {
        pattern_swing[i] = (int)sizeof(semitones);
      } else {
        pattern_swing[i] += amnt;
      }
    }
  }
}
void increment_swing(int amnt) {
  if (global_swing + amnt > (int)sizeof(semitones)) {
    global_swing = 0;
  } else if (global_swing + amnt < 0) {
    global_swing = (int)sizeof(semitones);
  } else {
    global_swing += amnt;
  }
}
void increment_key_glide(int amnt) {
  for (int i = 0; i < (int)sizeof(keypads_down); i++) {
    if (keypads_down[i]) {
      if (pattern_glide[i] + amnt > (int)sizeof(semitones)) {
        pattern_glide[i] = 0;
      } else if (pattern_glide[i] + amnt < 0) {
        pattern_glide[i] = (int)sizeof(semitones);
      } else {
        pattern_glide[i] += amnt;
      }
    }
  }
}
void increment_glide(int amnt) {
  if (global_glide + amnt > (int)sizeof(semitones)) {
    global_glide = 0;
  } else if (global_glide + amnt < 0) {
    global_glide = (int)sizeof(semitones);
  } else {
    global_glide += amnt;
  }
}
