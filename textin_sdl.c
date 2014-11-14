#include "global.h"
#include "textlist.h"
#include "timer.h"
#include "espeak.h"
#include "scorelist.h"

#define FALSE 0
#define TRUE 1

#define FPS 15

int quit = FALSE;
int max_inputs = 10;
int inputs_count = 0;

SDL_Surface *screen;
SDL_Surface *header_message;
SDL_Surface *footer_message;
SDL_Surface *input_text;
SDL_Surface *timer_text;
SDL_Surface *info_text;

char input_str[256];
wchar_t input_str_w[256];
int input_str_len = 0;

int screen_width = 800;
int screen_height = 600;
int screen_bpp = 32;

char window_header_str[256] = "TextIn";
char window_footer_str[256] = "Martin Kelm, 2014";

char font_file[128] = "resources/beyourself.ttf";
TTF_Font *font = NULL;
SDL_Color font_color = { 255, 255, 255 };
int font_size = 24;
TTF_Font *input_font = NULL;
int input_font_size = 64;

SDL_Event event;

void input_init() {
  input_font = TTF_OpenFont(font_file, input_font_size);

  strncpy(input_str, "", strlen(input_str));
  wcsncpy(input_str_w, L"", wcslen(input_str_w));
  input_text = NULL;
  SDL_EnableUNICODE(SDL_ENABLE);
}

int init_messages() {
  footer_message = TTF_RenderText_Solid(font, window_footer_str, font_color);
  if (footer_message == NULL) {
    return FALSE;
  }
  header_message = TTF_RenderText_Solid(font, window_header_str, font_color);
  if (header_message == NULL) {
    return FALSE;
  }
  return TRUE;
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

  font = TTF_OpenFont(font_file, font_size);
  if (font == NULL) {
    return FALSE;
  }

  char wt_temp[256] = "";
  strcat(wt_temp, window_header_str);
  strcat(wt_temp, " - ");
  strcat(wt_temp, window_footer_str);
  SDL_WM_SetCaption(wt_temp, NULL);

  input_init();

  textlist_init();
  textlist_set_random_pos();

  scorelist_load();

  espeak_init();

  timer_start();

  return init_messages();
}

void input_clean_up() {
  TTF_CloseFont(input_font);
  SDL_FreeSurface(input_text);
  SDL_EnableUNICODE(SDL_DISABLE);
}

void clean_up() {
  espeak_clean_up();

  input_clean_up();

  SDL_FreeSurface(info_text);
  SDL_FreeSurface(timer_text);
  SDL_FreeSurface(header_message);
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
  hlineColor(
    screen,
    screen_width * 0.1, screen_width * 0.9, screen_height / 2 + font_size,
    0xFFFFFFFF
  );
}

void handle_input() {
  if (event.type == SDL_KEYDOWN) {
    int input_change = FALSE;

    if (input_str_len < 256) {
      if (event.key.keysym.unicode == (Uint16)' ' ||
          (event.key.keysym.unicode >= (Uint16)'a' &&
           event.key.keysym.unicode <= (Uint16)'z') ||
          event.key.keysym.unicode == 223 || // ß
          event.key.keysym.unicode == 252 || // ü
          event.key.keysym.unicode == 228 || // ä
          event.key.keysym.unicode == 246) { // ö

        sprintf(input_str, "%s%c", input_str, event.key.keysym.unicode);
        wchar_t tmp[256];
        swprintf(tmp, 256, L"%lc", event.key.keysym.unicode);
        wcscat(input_str_w, tmp);

        input_str_len++;
        input_change = TRUE;
      }
    }
    if (event.key.keysym.sym == SDLK_BACKSPACE && input_str_len > 0) {
      input_str_len--;
      input_str[strlen(input_str)-1] = '\0';
      input_str_w[wcslen(input_str_w)-1] = '\0';
      input_change = TRUE;
    }
    if (input_change == TRUE) {
      SDL_FreeSurface(input_text);
      input_text = TTF_RenderText_Solid(input_font, input_str, font_color);
    }
  }
}

void show_timer_text() {
  SDL_FreeSurface(timer_text);
  char time[128];
  sprintf(time, "%d", timer_get_seconds());
  timer_text = TTF_RenderText_Solid(input_font, time, font_color);
  apply_surface(
    screen_width - timer_text->w, 0, timer_text, screen
  );
}

void show_info_text(int valid_input, int do_input_name) {
  SDL_FreeSurface(info_text);
  char info[256];
  if (do_input_name == TRUE) {
    sprintf(info, "bitte Namen eingeben");
  } else {
    sprintf(info, "gesucht %d Buchstaben", textlist_get_current_text_len());
    if (valid_input > -1) {
      sprintf(info, "%s - letzte Eingabe %s",
        info, (valid_input == TRUE) ? "richtig" : "falsch");
    } else {
      sprintf(info, "%s - keine Eingabe bisher", info);
    }
  }
  info_text = TTF_RenderText_Solid(font, info, font_color);
  apply_surface(
    screen_width / 2 - info_text->w / 2, screen_height / 2 + 1.2 * font_size,
    info_text, screen
  );
}

int main(int argc, char* args[]) {
  setlocale(LC_ALL, "de_DE.UTF-8");

  if (init() == FALSE)
    return 1;

  Uint32 frameStart = 0;
  espeak_set_run(TRUE);
  int valid_input = -1;
  int do_input_name = FALSE;
  while (quit == FALSE) {
    if (do_input_name == FALSE)
      timer_update();
    frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {

        int skip_input = (strcmp(input_str, "") == 0) ? TRUE : FALSE;
        if (do_input_name == TRUE && strcmp(input_str, "") != 0) {
          wprintf(
            L"--> %ls\n",
            scorelist_get_score_string(
              scorelist_add_score(input_str_w, textlist_get_chars_count(), timer_get_seconds()),
              TRUE
            )
          );

        } else if (textlist_current_compare(input_str_w) == 0 || skip_input == TRUE) {
          if (skip_input == FALSE)
            valid_input = TRUE;
          inputs_count++;
          espeak_lock();
          textlist_remove_current(skip_input);
          espeak_unlock();

          if (inputs_count == max_inputs) {
            wprintf(L"max inputs count reached\n");
            do_input_name = TRUE;
            input_init();
            //if (input_continue() == FALSE) {
              //quit = TRUE;
            //} else {
             /* textlist_init();
              textlist_set_random_pos();
              timer_start();
              inputs_count = 0; */
            //}
          }
          if (do_input_name == FALSE && quit == FALSE) {
            espeak_lock();
            quit = (textlist_set_random_pos() == TRUE) ? FALSE : TRUE;
            espeak_unlock();
            input_init();
            espeak_set_run(TRUE);
          }
        } else {
          valid_input = FALSE;
          input_init();
          espeak_set_run(TRUE);
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

    show_timer_text();
    show_info_text(valid_input, do_input_name);
    input_show_centered();

    apply_surface(
      screen_width - footer_message->w, screen_height - footer_message->h,
      footer_message, screen
    );
    apply_surface(0, 0, header_message, screen);
    if (SDL_Flip(screen) == -1)
      return 1;

    if ((SDL_GetTicks() - frameStart) < (1000 / FPS)) {
      SDL_Delay((1000 / FPS) - (SDL_GetTicks() - frameStart));
    }
  }

  clean_up();
  return 0;
}
