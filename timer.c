#include "global.h"
#include "timer.h"

Uint32 timer_i_start;
Uint32 timer_i_current;

void timer_start() {
  timer_i_start = SDL_GetTicks();
  timer_i_current = 0;
}

void timer_update() {
  timer_i_current = SDL_GetTicks() - timer_i_start;
}

Uint32 timer_get_seconds() {
  return (timer_i_current / 1000);
}
