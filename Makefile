SRCS=saloon.c
CFLAGS=-Wall -Wextra -masm=intel
ASLR=-pie -fPIE
RELRO=-Wl,-z,relro,-z,now
COOKIE=
LIBS=
CC=gcc
OUT=saloon

all:
	$(CC) $(CFLAGS) $(ASLR) $(RELRO) $(COOKIE) $(LIBS) $(SRCS) -o $(OUT)

debug:
	$(CC) $(CFLAGS) $(SRCS) -DDEBUG -g -o $(OUT)_d
