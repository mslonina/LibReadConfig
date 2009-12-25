NAME = readconfig
CC = gcc
CCFLAGS = -Wall
RM = rm -vf

all: lib demo main

lib:
	$(CC) $(CCFLAGS) -c libreadconfig.c -o libreadconfig-static.o
	ar rcs libreadconfig-static.a libreadconfig-static.o

demo:
	$(CC) $(CCFLAGS) -c readconfig.c -o readconfig.o

main:
	$(CC) $(CCFLAGS) -o $(NAME) readconfig.o -L. -lreadconfig-static 

clean:
	$(RM) $(NAME) *.o *.a 
