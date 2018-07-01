CC=gcc
CFLAGS=-O2 -Iinclude -ggdb
WFLAGS=-Wall -Wextra -pedantic
LDFLAGS=
SDL2_LDFLAGS=-lSDL2
SDL2_CFLAGS=-I/usr/include/SDL2

undat:
	$(CC) -lz $(CFLAGS) $(WFLAGS) src/dataio/{popcnt,endian,text,dat}.c utils/fr_undat.c -o undat


rix:
	$(CC) $(SDL2_LDFLAGS) $(CFLAGS) $(WFLAGS) $(SDL2_CFLAGS) -Iinclude src/dataio/endian.c src/rix/rix.c test/test_rix.c -o rix $(LDFLAGS)

clean:
	rm -f undat rix
