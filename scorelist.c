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

char scorelist_file[1024] = "resources/scorelist.txt";

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

void scorelist_save() {
  int i;
  wchar_t chunk[1024];
  FILE *fp = fopen(scorelist_file, "w, ccs=UTF-8");
  if (fp != NULL) {
    for (i = 0; i < scorelist_length; i++) {
      swprintf(
        chunk,
        1024,
        L"%ls %u %u %u\n",
        scorelist[i].name,
        scorelist[i].points, scorelist[i].chars, scorelist[i].seconds
      );
      fputws(chunk, fp);
    }
    fclose(fp);
  }
}

void scorelist_load() {
  int i, j;
  wchar_t chunk[1024];
  wchar_t delim[2] = L" ";
  wchar_t *ptr;
  wchar_t *token;

  FILE *fp = fopen(scorelist_file, "r, ccs=UTF-8");
  if (fp != NULL) {
    i = 0;
    while (fgetws(chunk, 1024, fp) != NULL) {
      chunk[wcslen(chunk) - 1] = L'\0';
      if (wcslen(chunk) > 0) {
        j = 0;
        token = wcstok(chunk, delim, &ptr);
        while (token != NULL) {
          switch (j) {
            case 0:
              wcsncpy(scorelist[i].name, token, 255);
              break;
            case 1:
              scorelist[i].points = (unsigned int)wcstoul(token, NULL, 0);
              break;
            case 2:
              scorelist[i].chars = (unsigned int)wcstoul(token, NULL, 0);
              break;
            case 3:
              scorelist[i].seconds = (unsigned int)wcstoul(token, NULL, 0);
              break;
          }
          token = wcstok(ptr, delim, &ptr);
          j++;
        }
        i++;
      }
    }
    fclose(fp);
    scorelist_length = i;
  }

  free(ptr);
  free(token);
}
