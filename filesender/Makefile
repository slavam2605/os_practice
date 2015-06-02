all: filesender

filesender: filesender.c
	LD_LIBRARY_PATH="../lib"
	gcc -D_POSIX_C_SOURCE -D_GNU_SOURCE -g filesender.c -std=c99 -L../lib/ -Wl,-rpath=../lib -lhelpers -lbufio -o filesender

clean:
	rm -f filesender
