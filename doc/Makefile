all:
	gcc -c lrc.c -o lrc.o
	gcc -c lrc-read.c -o lrc-read.o
	gcc lrc.o -o lrc -lhdf5 -lreadconfig
	gcc lrc-read.o -o lrc-read -lhdf5 -lreadconfig
