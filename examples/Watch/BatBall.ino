// A bat and ball game where the objective is to keep the ball
// from reaching the bottom of the screen.
// The bat is moved left and right by pressing the corresponding
// buttons on the side of the watch.

#define BB_LED_BACKGROUND 0
#define BB_LED_DIM 64
#define BB_LED_BRIGHT 255

#define BB_MIN_X 0
#define BB_MAX_X 7

#define BB_MIN_Y 1
#define BB_MAX_Y 7

#define BB_BAT_START_X 4
#define BB_BAT_START_Y 7

#define BB_BALL_START_X 3
#define BB_BALL_START_Y 4

#define BB_BALL_START_VELOCITY_X 1
#define BB_BALL_START_VELOCITY_Y -1

static int bbBatX = BB_BAT_START_X,
  bbBatY = BB_BAT_START_Y,
  bbBallX = BB_BALL_START_X,
  bbBallY = BB_BALL_START_Y,
  bbBallVelocityX = BB_BALL_START_VELOCITY_X,
  bbBallVelocityY = BB_BALL_START_VELOCITY_Y,
  bbBallDelta = 0;

static boolean bbBallMoving = false;

void mode_batball(uint8_t action) {
  uint8_t ledBright, ledDim, depth;
  uint16_t t;
  
  if(action != ACTION_NONE) {
    // If we just arrived here (whether through mode change
    // or wake from sleep), initialize the matrix driver:
    if(action >= ACTION_HOLD_LEFT) {
      uint8_t plex = LED_PLEX_2;
      depth = 4;
      // Reduce depth/plex if battery voltage is low
      if(watch.getmV() < BATT_LOW_MV) {
        depth = 2;
        plex  = LED_PLEX_1;
      }
      // Reconfigure display if needed
      if((watch.getDepth() != depth) || (watch.getPlex() != plex))
        fps = watch.setDisplayMode(depth, plex, true);
    }
    
    // Start the game when either button is tapped.
    if ((action == ACTION_TAP_LEFT) || (action == ACTION_TAP_RIGHT))
    {
      bbBallMoving = true;
    }
    
    // Move the bat within pre-defined bounds.
    if (action == ACTION_TAP_LEFT)
    {
      if (bbBatX > (BB_MIN_X + 1))
      {
        bbBatX--;
      }
    }
    else if (action == ACTION_TAP_RIGHT)
    {
      if (bbBatX < (BB_MAX_X - 1))
      {
        bbBatX++;
      }
    }
    
    // Reset sleep timeout on ANY button action.
    watch.setTimeout(fps * 16);
  }

  // Calc bright/dim LED colors based on current fadeout value.
  depth = watch.getDepth();
  if((t = watch.getTimeout()) < sizeof(fade)) {
    uint16_t b1 = (uint8_t)pgm_read_byte(&fade[t]) + 1;
    ledBright = (BB_LED_BRIGHT * b1) >> (16 - depth);
    ledDim = (BB_LED_DIM * b1) >> (16 - depth);
  } else {
    ledBright = BB_LED_BRIGHT >> (8 - depth);
    ledDim = BB_LED_DIM >> (8 - depth);
  }

  watch.fillScreen(BB_LED_BACKGROUND);
  
  // Draw ceiling.
  watch.drawPixel(0, 0, ledBright);
  watch.drawPixel(1, 0, ledDim);
  watch.drawPixel(2, 0, ledBright);
  watch.drawPixel(3, 0, ledDim);
  watch.drawPixel(4, 0, ledBright);
  watch.drawPixel(5, 0, ledDim);
  watch.drawPixel(6, 0, ledBright);
  watch.drawPixel(7, 0, ledDim);
  
  // Draw bat.
  watch.drawPixel(bbBatX, bbBatY, ledBright);
  watch.drawPixel((bbBatX + 1), bbBatY, ledBright);
  watch.drawPixel((bbBatX - 1), bbBatY, ledBright);
  
  // Draw ball.
  watch.drawPixel(bbBallX, bbBallY, ledBright);
  
  if (bbBallMoving)
  {
    // Ball movement based on number of frames passed.
    bbBallDelta++;
    
    if (bbBallDelta > 32)
    {
      bbBallX += bbBallVelocityX;
      bbBallY += bbBallVelocityY;
      
      bbBallDelta = 0;
    }
    
    // If the ball hits either wall, invert velocity on the X axis.
    if ((bbBallX <= BB_MIN_X) || (bbBallX >= BB_MAX_X))
    {
      bbBallVelocityX = -bbBallVelocityX;
    }
    
    // If the ball hits the ceiling or the bat, invert velocity on the Y axis.
    if ((bbBallY <= BB_MIN_Y) || ((bbBallY == 6) && (bbBallX >= bbBatX - 1) && (bbBallX <= bbBatX + 1)))
    {
      bbBallVelocityY = -bbBallVelocityY;
    }
    // If the ball leaves the bottom of the screen, the game restarts.
    else if (bbBallY >= BB_MAX_Y + 1)
    {
      bbBallMoving = false;
      
      bbBallX = BB_BALL_START_X;
      bbBallY = BB_BALL_START_Y;
      
      bbBatX = BB_BAT_START_X;
      bbBatY = BB_BAT_START_Y;
      
      bbBallVelocityX = BB_BALL_START_VELOCITY_X;
      bbBallVelocityY = BB_BALL_START_VELOCITY_Y;
    }
  }
}