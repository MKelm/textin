#include "global.h"
#include "textlist.h"
#include "SDL/SDL_thread.h"

#define FALSE 0
#define TRUE 1

#define FPS 15

int quit = FALSE;

SDL_Surface *screen;
SDL_Surface *footer_message;
SDL_Surface *input_text;

char input_str[256];
int input_str_len = 0;

int screen_width = 800;
int screen_height = 600;
int screen_bpp = 32;

char window_title_str[256] = "TextIn";
char window_footer_str[256] = "Martin Kelm, 2014";

char font_file[128] = "resources/beyourself.ttf";
TTF_Font *font = NULL;
SDL_Color font_color = { 255, 255, 255 };
int font_size = 24;

SDL_Event event;

SDL_Thread *thread1 = NULL;
int use_espeak_thread = FALSE;
SDL_sem *espeak_lock = NULL;

int espeak_thread(void *data) {
  int local_use_espeak = FALSE;
  while (quit == FALSE) {

    SDL_SemWait(espeak_lock);
    local_use_espeak = use_espeak_thread;
    SDL_SemPost(espeak_lock);

    if (local_use_espeak == TRUE) {
      char espeak_command[256] = "espeak -v mb/mb-de4 -s 90 \"";
      strcat(espeak_command, textlist_get_current());
      strcat(espeak_command, "\" 2>/dev/null");
      system(espeak_command);
      SDL_Delay(500);
    } else {
      SDL_Delay(1);
    }
  }

  return 0;
}

void input_init() {
  strncpy(input_str, "", sizeof(input_str));
  input_text = NULL;
  SDL_EnableUNICODE(SDL_ENABLE);
}

int init() {
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    return FALSE;
  }

  screen = SDL_SetVideoMode(
    screen_width, screen_height, screen_bpp, SDL_SWSURFACE | SDL_RESIZABLE
  );
  if (screen == NULL) {
    return FALSE;
  }

  if (TTF_Init() == -1) {
    return FALSE;
  }

  SDL_WM_SetCaption(window_title_str, NULL);

  input_init();

  thread1 = SDL_CreateThread(espeak_thread, NULL);

  return TRUE;
}

int load_files() {
  font = TTF_OpenFont(font_file, font_size);
  if (font == NULL) {
    return FALSE;
  }

  return TRUE;
}

int set_footer_message() {
  footer_message = TTF_RenderText_Solid(font, window_footer_str, font_color);
  if (footer_message == NULL) {
    return FALSE;
  }
  return TRUE;
}

void input_clean_up() {
  SDL_FreeSurface(input_text);
  SDL_EnableUNICODE(SDL_DISABLE);
}

void clean_up() {
  SDL_WaitThread(thread1, NULL);
  SDL_DestroySemaphore(espeak_lock);

  input_clean_up();

  SDL_FreeSurface(footer_message);

  TTF_CloseFont(font);
  TTF_Quit();

  SDL_Quit();
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination) {
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;

  SDL_BlitSurface(source, NULL, destination, &offset);
}

void input_show_centered() {
  if (input_text != NULL) {
    apply_surface(
      (screen_width - input_text->w) / 2, (screen_height - input_text->h) / 2,
      input_text, screen
    );
  }
}


void handle_input() {
  if(event.type == SDL_KEYDOWN) {
    char temp[256];
    sprintf(temp, "%s", input_str);

    if (input_str_len < 256) {
      printf("%d\n", event.key.keysym.unicode);
      if (event.key.keysym.unicode == (Uint16)' ' ||
          (event.key.keysym.unicode >= (Uint16)'a' &&
           event.key.keysym.unicode <= (Uint16)'z') ||
          event.key.keysym.unicode == 223 || // ß
          event.key.keysym.unicode == 252 || // ü
          event.key.keysym.unicode == 228 || // ä
          event.key.keysym.unicode == 246) { // ö
        sprintf(temp, "%s%c", temp, event.key.keysym.unicode);
        input_str_len++;
      }
    }
    if (event.key.keysym.sym == SDLK_BACKSPACE && input_str_len > 0) {
      printf("input len %d\n", input_str_len);
      input_str_len--;
      char temp2[256];
      strncpy(temp2, "", sizeof(temp2));
      if (input_str_len > 0) {
        strncpy(temp2, temp, input_str_len);
      }
      strncpy(temp, temp2, sizeof(temp));
    }
    if (strcmp(temp, input_str) != 0 ) {
      SDL_FreeSurface(input_text);
      strncpy(input_str, temp, sizeof(input_str));
      input_text = TTF_RenderText_Solid(font, input_str, font_color);
    }
  }
}

int main(int argc, char* args[]) {
  if (init() == FALSE)
    return 1;
  if (load_files() == FALSE)
    return 1;
  if (set_footer_message() == FALSE)
    return 1;

  // textlist tests
  textlist_load();
  textlist_set_random_pos();

  SDL_SemWait(espeak_lock);
  use_espeak_thread = TRUE;
  SDL_SemPost(espeak_lock);

  Uint32 frameStart = 0;
  while (quit == FALSE) {
    frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        printf("text entered\n");
        if (textlist_current_compare(input_str) == 0) {
          printf("match\n");
          SDL_SemWait(espeak_lock);
          use_espeak_thread = FALSE;
          SDL_SemPost(espeak_lock);

          textlist_remove_current();
          textlist_set_random_pos();
          input_init();

          SDL_SemWait(espeak_lock);
          use_espeak_thread = TRUE;
          SDL_SemPost(espeak_lock);

        } else {
          printf("no match\n");
        }
      } else {
        handle_input();
      }
      if (event.type == SDL_VIDEORESIZE) {
        screen_width = event.resize.w;
        screen_height = event.resize.h;
        screen = SDL_SetVideoMode(
          screen_width, screen_height, screen_bpp, SDL_SWSURFACE | SDL_RESIZABLE
        );
      }
      if (event.type == SDL_QUIT) {
        quit = TRUE;
      }
    }

    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

    input_show_centered();

    apply_surface(
      screen_width - footer_message->w, screen_height - footer_message->h,
      footer_message, screen
    );
    if (SDL_Flip(screen) == -1)
      return 1;

    if ((SDL_GetTicks() - frameStart) < (1000 / FPS)) {
      SDL_Delay((1000 / FPS) - (SDL_GetTicks() - frameStart));
    }
  }

  clean_up();
  return 0;
}
