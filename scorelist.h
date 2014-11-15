#define SCORELIST_MAX_LENGTH 5

struct st_scorelist {
  wchar_t name[256];
  unsigned int chars;
  unsigned int seconds;
  unsigned int points;
};

int scorelist_get_length();
int scorelist_get_next_pos(unsigned int points);
int scorelist_add_score(wchar_t *name, unsigned int chars, unsigned int seconds);

struct st_scorelist scorelist_get_score(int pos);
wchar_t *scorelist_get_score_string(int pos, int with_name);

void scorelist_save();
void scorelist_load();
