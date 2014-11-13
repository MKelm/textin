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

int scorelist_get_next_pos(unsigned int points) {
  if (scorelist_length < SCORELIST_MAX_LENGTH) {
    return scorelist_length;
  } else {
    int i, j;
    for (i = 0; i < scorelist_length; i++) {
      if (scorelist[i].points <= points) {
        for (j = i; j < scorelist_length - 1; j++) {
          scorelist[j+1] = scorelist[j];
        }
        return i;
      }
    }
  }
  return -1;
}

int scorelist_add_score(wchar_t *name, unsigned int chars, unsigned int seconds) {
  unsigned int points = (chars * 100.0) / seconds;
  int pos = scorelist_get_next_pos(points);
  if (pos > -1) {
    wcscpy(scorelist[pos].name, name);
    scorelist[pos].chars = chars;
    scorelist[pos].seconds = seconds;
    scorelist[pos].points = points;
    scorelist_length++;
  }
  return pos;
}

wchar_t *scorelist_get_score_string(int pos) {
  static wchar_t chunk[1024];
  if (pos > -1 && pos < scorelist_length) {
    swprintf(
      chunk,
      1024,
      L"%d Punkte (%d Buchstaben in %d Sekunden)",
       scorelist[pos].points, scorelist[pos].chars, scorelist[pos].seconds
    );
    return chunk;
  } else {
    return L"Kein Ranking.";
  }
}
