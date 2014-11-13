#include "global.h"
#include "textlist.h"
#include "timer.h"
#include "espeak.h"
#include "scorelist.h"

int quit = FALSE;
int max_inputs = 5;
int inputs_count = 0;

wchar_t input_str[256];
size_t input_str_len = 0;

void input_name() {
  wchar_t name[256];
  wprintf(L"--> Name: ");
  fgetws(name, 255, stdin);
  name[wcslen(name) - 1] = L'\0';
  scorelist_show_score(
    scorelist_add_score(name, textlist_get_chars_count(), timer_get_seconds())
  );
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
    timer_update();

    if (wcscmp(input_str, L"ende") == 0) {
      quit = TRUE;
    } else if (textlist_current_compare(input_str) == 0) {
      wprintf(L"--> %d. Eingabe richtig\n", inputs_count + 1);
      inputs_count++;

      if (inputs_count == max_inputs) {
        quit = TRUE;
        input_name();

      } else {
        espeak_lock();
        textlist_remove_current();
        quit = (textlist_set_random_pos() == TRUE) ? FALSE : TRUE;
        espeak_unlock();
      }
    } else {
      wprintf(L"--> Eingabe falsch\n");
    }
    if (quit == FALSE) {
      wprintf(L"--> Zeit: %u Sekunden\n", timer_get_seconds());
    }
  }

  espeak_clean_up();

  return 0;
}
