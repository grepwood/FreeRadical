CC=gcc
CFLAGS=-O2 -Iinclude -ggdb
WFLAGS=-Wall -Wextra -pedantic
LDFLAGS=

undat:
	$(CC) -lz $(CFLAGS) $(WFLAGS) src/dataio/{popcnt,endian,text,dat}.c utils/fr_undat.c -o undat

clean:
	rm -f undat
