
main_heap: heap.o main_heap.o 
	gcc main_heap.o heap.o  BF_64.a -no-pie -o main_heap
main_heap.o :main_heap.c 
	gcc -c main_heap.c
heap.o :heap.c
	gcc -c heap.c
clean:
	rm main_heap main_heap.o heap.o
