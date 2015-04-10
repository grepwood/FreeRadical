CC=gcc
#CFLAGS=-O2 -Iinclude -g3
#CFLAGS=-O2 -Iinclude -g3
#CFLAGS=-O2 -DBUILTIN_POPCNT -march=native -mtune=native -Iinclude -g3
CFLAGS=-O0 -march=native -mtune=native -Iinclude -g3
WFLAGS=-Wall -Wextra -pedantic
LDFLAGS=

undat:
	$(CC) -lz $(CFLAGS) $(WFLAGS) src/dataio/{popcnt,endian,text,dat}.c utils/fr_undat.c -o undat

clean:
	rm -f undat

release:
	strip undat
