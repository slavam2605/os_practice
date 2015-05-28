all:    
	gcc -shared -g -o libhelpers.so -std=c99 -fPIC helpers.c
	gcc -shared -g -o libbufio.so -std=c99 -fPIC bufio.c

clean:
	rm -f libhelpers.so
	rm -f libbufio.so
