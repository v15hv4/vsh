IDIR=include
SDIR=src
BDIR=build
ODIR=build/obj

CC=gcc
CFLAGS=-I$(IDIR)

vsh: $(ODIR)/vsh.o $(ODIR)/utils.o
	$(CC) $(CFLAGS) -g $(ODIR)/vsh.o $(ODIR)/utils.o -o $(BDIR)/vsh 
	chmod +x build/vsh

$(ODIR)/vsh.o: $(SDIR)/vsh.c
	$(CC) $(CFLAGS) -c $(SDIR)/vsh.c -o $(ODIR)/vsh.o

$(ODIR)/utils.o: $(SDIR)/utils.c
	$(CC) $(CFLAGS) -c $(SDIR)/utils.c -o $(ODIR)/utils.o

clean:
	rm -rf ./build

$(shell mkdir -p $(BDIR) $(ODIR))
