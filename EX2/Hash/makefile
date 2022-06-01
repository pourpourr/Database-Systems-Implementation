CC = gcc
OBJ = ysbd main.o hash.o records.o secHash.o
all:  ysbd



ysbd: main.o hash.o records.o secHash.o
	$(CC) -o ysbd main.o hash.o secHash.o records.o BF_64.a -no-pie



main.o:main.c
		$(CC)  main.c -c

records.o:records.c
		$(CC)  records.c -c

hash.o:hash.c
		$(CC)  hash.c -c

secHash.o:secHash.c
		$(CC)  secHash.c -c

clean:
	rm -f $(OBJ)   osfp pao
