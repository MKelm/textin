int espeak_thread_f(void *data);

void espeak_init();
void espeak_lock();
void espeak_unlock();
void espeak_set_run(int run);
void espeak_clean_up();
