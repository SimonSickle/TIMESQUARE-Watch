static int8_t board[8][8], nextBoard[8][8];
static uint32_t last_update;

void random_board(){
  int8_t x,y;
  for(x=0; x<8; x++) {
    for(y=0; y<8; y++) { board[x][y] = (random(100) > 50 ? 1 : 0); }
  }
  
  last_update = RTC.now().unixtime();
}

void update_board() {
  int8_t x,y;
  for(x=0; x<8; x++) {
    for(y=0; y<8; y++) {
      int8_t n_count = count_neighbors(x,y);
      if ((board[x][y] == 1) && ((n_count < 2) || (n_count > 3))) { nextBoard[x][y] = 0; } // death
      if ((board[x][y] == 1) && ((n_count == 2) || (n_count == 3))) { nextBoard[x][y] = 1; } // survival
      if ((board[x][y] == 0) && (n_count == 3)) { nextBoard[x][y] = 1; } // birth
    }
  }
  
  for(x=0; x<8; x++) {
    for(y=0; y<8; y++) {
      board[x][y] = nextBoard[x][y];
    }
  }
  
  last_update = RTC.now().unixtime();
}

int8_t count_neighbors(int8_t x, int8_t y) {
  int8_t n = 0;
  if ((x > 0) && (y > 0)) { n += board[(x-1)][(y-1)]; }
  if (y > 0) { n += board[(x)][(y-1)];}
  if (x > 0) { n += board[(x-1)][(y)];}
  if ((x > 0) && (y < 7)) { n += board[(x-1)][(y+1)];}
  if ((x < 7) && (y < 7)) { n += board[(x+1)][(y+1)];}
  if (y < 7) { n += board[(x)][(y+1)];}
  if (x < 7) { n += board[(x+1)][(y)];}
  if ((x < 7) && (y > 0)) { n += board[(x+1)][(y-1)];}
  return n;
}

void mode_life(uint8_t action) {
  if(action != ACTION_NONE) {
    if(action >= ACTION_HOLD_LEFT) {      
      uint8_t depth = 6, plex = LED_PLEX_4;
      // Reduce depth/plex if battery voltage is low
      if(watch.getmV() < BATT_LOW_MV) {
        depth = 2;
        plex  = LED_PLEX_1;
      }
      // Reconfigure display if needed
      if((watch.getDepth() != depth) || (watch.getPlex() != plex))
        fps = watch.setDisplayMode(depth, plex, true);
        
      random_board();
    } else if((action == ACTION_TAP_LEFT) || (action == ACTION_TAP_RIGHT)) {
      update_board();
    }
    // Reset sleep timeout on ANY button action
    watch.setTimeout(fps * 30);
  }
  
  uint32_t now = RTC.now().unixtime();
  
  if ((now - last_update) > 1) { update_board(); }
  
  watch.fillScreen(0);
  
  int8_t x,y;
  // Draw board
  for(x=0; x<8; x++) {
    for(y=0; y<8; y++) { watch.drawPixel(x,y, (board[x][y] == 1) ? 255 : 0); }
  }
}