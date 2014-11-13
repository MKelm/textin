#include "global.h"
#include "espeak.h"
#include "textlist.h"

extern int quit;

int espeak_run = FALSE;

SDL_Thread *espeak_thread = NULL;
SDL_mutex *espeak_lock_m = NULL;

int espeak_thread_f(void *data) {
  int run = FALSE;

  while (quit == FALSE) {
    espeak_lock();
    run = espeak_run;
    espeak_unlock();

    if (run == TRUE) {
      espeak_set_run(FALSE);

      wchar_t espeak_command_w[256] = L"espeak -v mb/mb-de4 -s 90 \"";
      wcscat(espeak_command_w, textlist_get_current());
      wcscat(espeak_command_w, L"\" 2>/dev/null");
      char espeack_command[256];
      wcstombs(espeack_command, espeak_command_w, 255);
      system(espeack_command);

    } else {
      SDL_Delay(1);
    }
  }
  return 0;
}

void espeak_init() {
  espeak_lock_m = SDL_CreateMutex();
  espeak_thread = SDL_CreateThread(espeak_thread_f, NULL);
}

void espeak_lock() {
  SDL_mutexP(espeak_lock_m);
}

void espeak_unlock() {
  SDL_mutexV(espeak_lock_m);
}

void espeak_set_run(int run) {
  espeak_lock();
  espeak_run = run;
  espeak_unlock();
}

void espeak_clean_up() {
  SDL_WaitThread(espeak_thread, NULL);
  SDL_DestroyMutex(espeak_lock_m);
}
