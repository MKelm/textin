#include "global.h"
#include "scorelist.h"

#define SCORELIST_MAX_LENGTH 5

struct st_scorelist {
  wchar_t name[256];
  unsigned int chars;
  unsigned int seconds;
  unsigned int points;
} scorelist[SCORELIST_MAX_LENGTH];

int scorelist_length = 0;

int scorelist_get_length() {
  return scorelist_length;
}

int scorelist_get_next_pos(unsigned int points) {
  if (scorelist_length == 0)
    return scorelist_length;
  int i, j;
  for (i = 0; i < scorelist_length; i++) {
    if (scorelist[i].points <= points) {
      for (j = scorelist_length - 1; j > i; j--) {
        scorelist[j] = scorelist[j-1];
      }
      return i;
    }
  }
  if (scorelist_length < SCORELIST_MAX_LENGTH)
    return scorelist_length;
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
    if (scorelist_length < SCORELIST_MAX_LENGTH) {
      scorelist_length++;
    }
  }
  return pos;
}

wchar_t *scorelist_get_score_string(int pos, int with_name) {
  static wchar_t chunk[1024];
  if (pos > -1 && pos < scorelist_length) {
    swprintf(
      chunk,
      1024,
      L"%ls%ls%u Punkte (%u Buchstaben in %u Sekunden)",
      ((with_name == TRUE) ? scorelist[pos].name : L""),
      ((with_name == TRUE) ? L", " : L""),
      scorelist[pos].points, scorelist[pos].chars, scorelist[pos].seconds
    );
    return chunk;
  } else {
    return L"Leider keinen Punkteplatz erhalten";
  }
}
