# Makefile for textin
ARCH := $(shell uname -m)
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lSDL -lSDL_image -lSDL_gfx -lSDL_ttf

textin_sdl : textin_sdl.o textlist.o
	$(CC) textin_sdl.o textlist.o -o textin_sdl.$(ARCH) $(LDFLAGS)

textin_sdl.o : textin_sdl.c textlist.h
	$(CC) $(CFLAGS) -c textin_sdl.c

textlist.o : textlist.c textlist.h
	$(CC) $(CFLAGS) -c textlist.c

clean :
	$(RM) *.o
