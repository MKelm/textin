#include "global.h"
#include "scorelist.h"

#define SCORELIST_MAX_LENGTH 10

struct st_scorelist {
  wchar_t name[256];
  unsigned int chars;
  unsigned int seconds;
  unsigned int points;
} scorelist[SCORELIST_MAX_LENGTH];

int scorelist_length = 0;

int scorelist_add_score(wchar_t *name, unsigned int chars, unsigned int seconds) {
  wcscpy(scorelist[scorelist_length].name, name);
  scorelist[scorelist_length].chars = chars;
  scorelist[scorelist_length].seconds = seconds;
  scorelist[scorelist_length].points =
    scorelist[scorelist_length].chars / scorelist[scorelist_length].seconds;
  scorelist_length++;
  return scorelist_length - 1;
}

void scorelist_show_score(int pos) {
  wprintf(
    L"--> %d Punkte (%d Buchstaben in %d Sekunden)\n",
    scorelist[pos].points, scorelist[pos].chars, scorelist[pos].seconds
  );
}
