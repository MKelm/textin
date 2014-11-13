#include "global.h"
#include "textlist.h"
#include "timer.h"
#include "espeak.h"

int quit = FALSE;

wchar_t input_str[256];
size_t input_str_len = 0;

void input_init() {
  wcscpy(input_str, L"");
}

int main() {
  setlocale(LC_ALL, "de_DE.UTF-8");

  textlist_load();
  textlist_set_random_pos();

  espeak_init();

  timer_start();
  while (quit == FALSE) {
    espeak_set_run(TRUE);

    fgetws(input_str, 255, stdin);
    input_str[wcslen(input_str) - 1] = L'\0';
    if (wcscmp(input_str, L"ende") == 0) {
      quit = TRUE;
    } else if (textlist_current_compare(input_str) == 0) {
      wprintf(L"--> Eingabe richtig\n");

      espeak_lock();;
      textlist_remove_current();
      quit = (textlist_set_random_pos() == TRUE) ? FALSE : TRUE;
      espeak_unlock();

      if (quit == FALSE) {
        input_init();
      }
    } else {
      wprintf(L"--> Eingabe falsch\n");
    }
    timer_update();
    wprintf(L"--> Zeit: %u Sekunden\n", timer_get_seconds());
  }

  espeak_clean_up();

  return 0;
}
