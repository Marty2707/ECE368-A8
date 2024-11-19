WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g $(WARNING) $(ERROR)

a5: a8.o
	$(GCC) a8.o -o a8

a5.o: a8.c
	$(GCC) -c a8.c -o a8.o

clean:
	rm -f *.o a8
