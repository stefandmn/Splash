NAME=splash
CC=gcc

# Program distribution structure
PREFIX=/opt/clue
BINDIR=${PREFIX}/bin
SHRDIR=${PREFIX}/share/splash

# These variables are for Linux compiler
LIBS=-lfreetype -ljpeg -lpng12 -lm -lz
INCL=-I/usr/include/freetype2 -I/usr/include/freetype2/freetype
ARCH=linux


make:
	if [ -d out/ ]; then rm -rf out/; mkdir -p out/; else mkdir -p out/; fi
	${CC} -w -static *.c -o out/${NAME} ${LIBS} ${INCL}


install: make
	if [ ! -d ${BINDIR} ]; then mkdir -p ${BINDIR} ; fi
	if [ ! -d ${SHRDIR} ]; then mkdir -p ${SHRDIR} ; fi
	cp -rf out/${NAME} ${BINDIR}/${NAME}
	cp -rf res/* ${SHRDIR}


clean: 
	rm -rf ./out/
