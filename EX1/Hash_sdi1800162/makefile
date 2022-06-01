CC = gcc
OBJ = ysbd main.o hash.o records.o
all:  ysbd



ysbd: main.o hash.o records.o
	$(CC) -o ysbd main.o hash.o records.o BF_64.a -no-pie



main.o:main.c
		$(CC)  main.c -c

records.o:records.c
		$(CC)  records.c -c

hash.o:hash.c
		$(CC)  hash.c -c


clean:
	rm -f $(OBJ)   osfp
