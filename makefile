CPP=gcc
CFLAGS=-O3
LDFLAGS=-lSDL -lSDLmain -lSDL_mixer -lSDL_image -lSDL_ttf
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
