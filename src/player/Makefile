CC=gcc
CFLAGS=-g

SDL2FLAGS=`sdl2-config --cflags`
SDL2LIBS=`sdl2-config --libs`

SDLFLAGS=`sdl-config --cflags`
SDLLIBS=`sdl-config --libs`

LDFLAGS=-lwaave

all:
	echo "Targets: sdl sdl2 clean"
sdl:
	$(CC) $(CFLAGS) $(SDLFLAGS) waaveplayer12.c -o waaveplayer12 $(SDLLIBS) $(LDFLAGS)
sdl2:
	$(CC) $(CFLAGS) $(SDL2FLAGS) waaveplayer20.c -o waaveplayer20 $(SDL2LIBS) $(LDFLAGS)
clean:
	rm -f waaveplayer12 waaveplayer20
