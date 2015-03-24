all:
	LD_LIBRARY_PATH="../lib"
	gcc cat.c -L../lib/ -Wl,-rpath=../lib -lhelpers -o cat

clean:
	rm cat
