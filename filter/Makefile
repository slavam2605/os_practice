all:
	LD_LIBRARY_PATH="../lib"
	gcc filter.c -L../lib/ -Wl,-rpath=../lib -lhelpers -o filter

clean:
	rm -f filter
