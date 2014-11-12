# Makefile for textin
ARCH := $(shell uname -m)
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lSDL -lSDL_image -lSDL_gfx -lSDL_ttf

textin : textin.o textlist.o
	$(CC) textin.o textlist.o -o textin.$(ARCH) $(LDFLAGS)

textin.o : textin.c textlist.h
	$(CC) $(CFLAGS) -c textin.c

textlist.o : textlist.c textlist.h
	$(CC) $(CFLAGS) -c textlist.c

clean :
	$(RM) *.o
