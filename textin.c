#include "global.h"
#include "textlist.h"

#define FALSE 0
#define TRUE 1

#define FPS 15

SDL_Surface *screen;
SDL_Surface *message;

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
  message = TTF_RenderText_Solid(font, window_footer_str, font_color);
  if (message == NULL) {
    return FALSE;
  }
  return TRUE;
}

void clean_up() {
  SDL_FreeSurface(message);

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
  char espeak_command[256] = "espeak -v mb/mb-de4 -s 90 \"";
  strcat(espeak_command, textlist_get_current());
  strcat(espeak_command, "\" 2>/dev/null");
  system(espeak_command);
  printf("speech_command = %s\n", espeak_command);
  textlist_remove_current();

  Uint32 frameStart = 0;
  int quit = FALSE;
  while (quit == FALSE) {
    frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {

      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          default: ;
        }
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

    apply_surface(
      screen_width - message->w, screen_height - message->h, message, screen
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
