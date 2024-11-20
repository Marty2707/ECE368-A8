# Compiler flags
WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g $(WARNING) $(ERROR)

# Target executable name
a8: a8.o
	$(GCC) a8.o -o a8

# Object file rule
a8.o: a8.c
	$(GCC) -c a8.c -o a8.o

# Clean rule to remove build artifacts
clean:
	rm -f *.o a8