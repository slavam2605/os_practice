all: forking

forking: forking.c
	LD_LIBRARY_PATH="../lib"
	gcc -D_POSIX_C_SOURCE -D_GNU_SOURCE -g forking.c -std=c99 -L../lib/ -Wl,-rpath=../lib -lhelpers -lbufio -o forking

clean:
	rm -f forking
