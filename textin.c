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
  wprintf(L"\n--> Name: ");
  fgetws(name, 255, stdin);
  name[wcslen(name) - 1] = L'\0';

  wprintf(
    L"--> %ls\n",
    scorelist_get_score_string(
      scorelist_add_score(name, textlist_get_chars_count(), timer_get_seconds())
    )
  );
}

int input_continue() {
  wchar_t continue_str[256];
  wprintf(L"\n--> Weiter? (j/n) ");
  fgetws(continue_str, 255, stdin);
  continue_str[wcslen(continue_str) - 1] = L'\0';
  if (wcscmp(continue_str, L"j") == 0) {
    return TRUE;
  }
  return FALSE;
}

int main() {
  setlocale(LC_ALL, "de_DE.UTF-8");

  textlist_init();
  textlist_set_random_pos();

  espeak_init();

  timer_start();
  while (quit == FALSE) {
    espeak_set_run(TRUE);
    wprintf(L"\n--> Gesuchtes Wort mit %d Buchstaben\n", textlist_get_current_text_len());
    wprintf(L"--> ");
    fgetws(input_str, 255, stdin);
    input_str[wcslen(input_str) - 1] = L'\0';
    timer_update();

    if (wcscmp(input_str, L"ende") == 0) {
      quit = TRUE;
    } else if (textlist_current_compare(input_str) == 0) {
      wprintf(L"--> %d. Eingabe richtig\n", inputs_count + 1);
      inputs_count++;
      wprintf(L"--> Zeit: %u Sekunden\n", timer_get_seconds());

      if (inputs_count == max_inputs) {
        input_name();
        if (input_continue() == FALSE) {
          quit = TRUE;
        } else {
          textlist_init();
          textlist_set_random_pos();
          timer_start();
          inputs_count = 0;
        }
      }
      if (quit == FALSE) {
        espeak_lock();
        textlist_remove_current();
        quit = (textlist_set_random_pos() == TRUE) ? FALSE : TRUE;
        espeak_unlock();
      }
    } else {
      wprintf(L"--> Eingabe falsch\n");
    }
  }

  espeak_clean_up();

  return 0;
}
