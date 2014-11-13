#include "global.h"
#include "textlist.h"
#include <time.h>

#define TEXTLIST_MAX_LENGTH 1024

struct st_textlist {
  wchar_t text[128];
  size_t text_len;
  int used;
} textlist[TEXTLIST_MAX_LENGTH];
int textlist_length;

int textlist_current = -1;
size_t textlist_chars_counter = 0;

void textlist_load() {
  srand(time(NULL));

  FILE *fp;
  int i = 0;
  fp = fopen("resources/textlist_de.txt", "r, ccs=UTF-8");
  if (fp != NULL) {
    wchar_t chunk[1024];
    while (fgetws(chunk, 1024, fp) != NULL) {
      chunk[wcslen(chunk) - 1] = L'\0';
      if (wcslen(chunk) > 0) {
        wcscpy(textlist[i].text, chunk);
        textlist[i].text_len = wcslen(chunk);
        textlist[i].used = 0;
        i++;
      }
    }
    fclose(fp);
  }
  textlist_length = i;
}

void textlist_set_random_pos() {
  textlist_current = rand() % textlist_length;
}

wchar_t *textlist_get_current() {
  if (textlist_current > -1)
    return textlist[textlist_current].text;
  else
    return L"";
}

int textlist_current_compare(wchar_t *text) {
  return wcscmp(textlist[textlist_current].text, text);
}

void textlist_remove_current() {
  textlist_chars_counter += textlist[textlist_current].text_len;
  int i;
  for (i = textlist_current; i < textlist_length - 1; i++) {
    textlist[i] = textlist[i+1];
  }
  textlist_length--;
  textlist_current = -1;
}
