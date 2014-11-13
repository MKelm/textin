#include "global.h"
#include "textlist.h"

#define FALSE 0
#define TRUE 1

int quit = FALSE;

wchar_t input_str[256];
size_t input_str_len = 0;

SDL_Thread *thread1 = NULL;
int use_espeak_thread = FALSE;
SDL_sem *espeak_lock = NULL;

int espeak_thread(void *data) {
  int local_use_espeak = FALSE;
  while (quit == FALSE) {

    SDL_SemWait(espeak_lock);
    local_use_espeak = use_espeak_thread;
    SDL_SemPost(espeak_lock);

    if (local_use_espeak == TRUE) {
      wchar_t espeak_command_w[256] = L"espeak -v mb/mb-de4 -s 90 \"";
      wcscat(espeak_command_w, textlist_get_current());
      wcscat(espeak_command_w, L"\" 2>/dev/null");
      char espeack_command[256];
      wcstombs(espeack_command, espeak_command_w, 255);
      system(espeack_command);
      SDL_Delay(500);
    } else {
      SDL_Delay(1);
    }
  }

  return 0;
}

int main() {
  setlocale(LC_ALL, NULL);
  wcscpy(input_str, L"");

  textlist_load();
  textlist_set_random_pos();

  use_espeak_thread = TRUE;
  thread1 = SDL_CreateThread(espeak_thread, NULL);

  while (quit == FALSE) {
    // todo input / output logic
    SDL_Delay(1);
  }

  SDL_WaitThread(thread1, NULL);
  SDL_DestroySemaphore(espeak_lock);

  return 0;
}
