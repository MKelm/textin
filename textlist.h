void textlist_init();
int textlist_set_random_pos();
size_t textlist_get_current_text_len();
wchar_t *textlist_get_current();
int textlist_current_compare(const wchar_t *text);
void textlist_remove_current(int skip_chars_count);
unsigned int textlist_get_chars_count();
