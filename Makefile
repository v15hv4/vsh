CC=gcc
CFLAGS=-I.

vsh: src/vsh.c
	mkdir build
	$(CC) $(CFLAGS) -o build/vsh src/vsh.c
	chmod +x build/vsh

clean:
	rm -rf ./build
