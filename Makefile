ifneq ($(GLX),)
CFLAGS=-DGLX -lX11
else
CFLAGS=-DSDL `sdl-config --cflags` `sdl-config --libs`
endif
rb:rb.c
	gcc -std=gnu99 -Os -march=native -s -ffast-math -fwhole-program -o rainbeam rb.c ${CFLAGS} -lGL