#include "global.h"
#include "textlist.h"
#include <time.h>

#define TEXTLIST_MAX_LENGTH 1024

struct st_textlist {
  char text[128];
  int text_len;
  int used;
} textlist[TEXTLIST_MAX_LENGTH];
int textlist_length;

int textlist_current = -1;
int textlist_chars_counter = 0;

int textlist_strlen_utf8(char *s) {
  int i = 0, j = 0;
  while (s[i]) {
    if ((s[i] & 0xc0) != 0x80) j++;
    i++;
  }
  return j;
}

void textlist_load() {
  srand(time(NULL));

  FILE *fp;
  int i = 0;
  fp = fopen("resources/textlist_de.txt", "r");
  if (fp != NULL) {
    char chunk[1024];
    while (fgets(chunk, 1024, fp) != NULL) {
      strtok(chunk, "\n");
      if (strlen(chunk) > 0) {
        strcpy(textlist[i].text, chunk);
        textlist[i].text_len = textlist_strlen_utf8(chunk);
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

char *textlist_get_current() {
  if (textlist_current > -1)
    return textlist[textlist_current].text;
  else
    return "";
}

int textlist_current_compare(char *text) {
  return strcmp(textlist[textlist_current].text, text);
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
