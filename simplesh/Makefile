all: simplesh

simplesh: simplesh.c
	LD_LIBRARY_PATH="../lib"
	gcc -D_POSIX_C_SOURCE -D_GNU_SOURCE -g simplesh.c -std=c99 -L../lib/ -Wl,-rpath=../lib -lhelpers -lbufio -o simplesh

clean:
	rm -f simplesh
