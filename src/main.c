#include "gba.h"
#include "main.h"
#include "player.h"
#include "start.h"
#include "item.h"
#include "enemy.h"
#include "win.h"
#include "lose.h"

#include <stdio.h>
#include <stdlib.h>

enum gba_state {
  START,
  PLAY,
  WIN,
  LOSE,
};
enum item_state {
  OPEN = 0,
  CLOSED = 1
};

struct player plyr = {75, 5, 10, 10};
struct enemy enms[] = {
  {20, 90, 10, 10, 1},
  {130, 110, 10, 10, -1},
  {20, 130, 10, 10, 1}
};
struct item items[] = {
  {20, 110, 10, 10, 0},
  {75, 110, 10, 10, 0},
  {130, 110, 10, 10, 0}
};
struct goal goal = {65, 230, 10, 30};
int score = 0;

void resetState(enum gba_state *state, enum gba_state st, struct player *p, struct item itm[], int *score);

int main(void) {
  // Manipulate REG_DISPCNT here to set Mode 3. //

  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons

    // Manipulate the state machine. //
    switch (state) {
      case START:
        waitForVBlank();
        drawFullScreenImageDMA(start);
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          state = PLAY;
        }
        break;
      
      case PLAY:
        waitForVBlank();
        fillScreenDMA(BLACK);
        drawImageDMA(plyr.r, plyr.c, plyr.w, plyr.h, player);
        drawRectDMA(goal.r, goal.c, goal.w, goal.h, RED);
        for (int i = 0; i < (int) (sizeof(items) / sizeof(items[0])); i++) {
          if (items[i].status == OPEN) {
            drawImageDMA(items[i].r, items[i].c, items[i].w, items[i].h, item);
          }
          if (items[i].status == OPEN &&
              plyr.r < items[i].r + items[i].h && plyr.r + plyr.h > items[i].r &&
              plyr.c < items[i].c + items[i].w && plyr.c + plyr.w > items[i].c) {
            items[i].status = CLOSED;
            score++;
          }
        }
        for (int i = 0; i < (int) (sizeof(enms) / sizeof(enms[0])); i++) {
          drawImageDMA(enms[i].r, enms[i].c, enms[i].w, enms[i].h, enemy);
          if (plyr.r < enms[i].r + enms[i].h && plyr.r + plyr.h > enms[i].r &&
              plyr.c < enms[i].c + enms[i].w && plyr.c + plyr.w > enms[i].c) {
            state = LOSE;
          }
          if ((enms[i].r == 20 && enms[i].dr < 0) || (enms[i].r == 130 && enms[i].dr > 0)) {
            enms[i].dr = -enms[i].dr;
          }
          enms[i].r += enms[i].dr;
        }
        drawString(150, 5, "Score: ", WHITE);
        drawChar(150, 42, score + 48, WHITE);
        if (plyr.r < goal.r + goal.h && plyr.r + plyr.h > goal.r &&
            plyr.c < goal.c + goal.w && plyr.c + plyr.w > goal.c) {
          if (score == 3) {
            state = WIN;
          }
        }
        if (KEY_DOWN(BUTTON_UP, currentButtons)) {
          if (plyr.r > 0) {
            plyr.r--;
          }
        }
        if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
          if (plyr.r < 150) {
            plyr.r++;
          }
        }
        if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
          if (plyr.c < 230) {
            plyr.c++;
          }
        }
        if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
          if (plyr.c > 0) {
            plyr.c--;
          }
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          resetState(&state, START, &plyr, items, &score);
        }
        break;

      case WIN:
        waitForVBlank();
        drawFullScreenImageDMA(win);
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          resetState(&state, PLAY, &plyr, items, &score);
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          resetState(&state, START, &plyr, items, &score);
        }
      break;
      
      case LOSE:
        waitForVBlank();
        drawFullScreenImageDMA(lose);
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          resetState(&state, PLAY, &plyr, items, &score);
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          resetState(&state, START, &plyr, items, &score);
        }
        break;
    }
    previousButtons = currentButtons; // Store the current state of the buttons
  }
  return 0;
}

void resetState(enum gba_state *state, enum gba_state st,
                struct player *plyr, struct item items[], int *score) {
  *state = st;
  plyr->r = 75, plyr->c = 5;
  items[0].status = OPEN, items[1].status = OPEN, items[2].status = OPEN;
  *score = 0;
}