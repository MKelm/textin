# Makefile for textin
ARCH := $(shell uname -m)
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lSDL -lSDL_image -lSDL_gfx -lSDL_ttf

textin : textin.o textlist.o timer.o espeak.o scorelist.o
	$(CC) textin.o textlist.o timer.o espeak.o scorelist.o -o textin.$(ARCH) $(LDFLAGS)

textin.o : textin.c textlist.h timer.h espeak.h scorelist.h global.h
	$(CC) $(CFLAGS) -c textin.c

textin_sdl : textin_sdl.o textlist.o
	$(CC) textin_sdl.o textlist.o -o textin_sdl.$(ARCH) $(LDFLAGS)

textin_sdl.o : textin_sdl.c textlist.h global.h
	$(CC) $(CFLAGS) -c textin_sdl.c

textlist.o : textlist.c textlist.h global.h
	$(CC) $(CFLAGS) -c textlist.c

timer.o : timer.c timer.h global.h
	$(CC) $(CFLAGS) -c timer.c

espeak.o : espeak.c espeak.h global.h
	$(CC) $(CFLAGS) -c espeak.c

scorelist.o : scorelist.c scorelist.h global.h
	$(CC) $(CFLAGS) -c scorelist.c

clean :
	$(RM) *.o
