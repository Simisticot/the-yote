CPP=gcc
CFLAGS=-O3
LDFLAGS=-Wall -Werror -lSDL -lSDLmain -lSDL_mixer -lSDL_image -lSDL_ttf -lSDL_gfx
EXEC=Yote

all: ${EXEC}

${EXEC}: ${EXEC}.o
	${CPP} $(CFLAGS) -o ${EXEC} ${EXEC}.o ${LDFLAGS}

${EXEC}.o: ${EXEC}.c
	${CPP} $(CFLAGS) -o ${EXEC}.o -c ${EXEC}.c


clean:	
	rm -fr *.o

mrproper: clean
	rm -fr ${EXEC}
