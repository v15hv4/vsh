IDIR=include
SDIR=src
BDIR=build
ODIR=build/obj

CC=gcc
CFLAGS=-I$(IDIR)

vsh: $(ODIR)/vsh.o $(ODIR)/utils.o $(ODIR)/errors.o $(ODIR)/path.o $(ODIR)/proc.o  $(ODIR)/prompt.o
	$(CC) $(CFLAGS) -g $(ODIR)/vsh.o $(ODIR)/utils.o $(ODIR)/errors.o $(ODIR)/path.o $(ODIR)/proc.o $(ODIR)/prompt.o -o $(BDIR)/vsh 
	chmod +x build/vsh

$(ODIR)/vsh.o: $(SDIR)/vsh.c
	$(CC) $(CFLAGS) -c $(SDIR)/vsh.c -o $(ODIR)/vsh.o

$(ODIR)/utils.o: $(SDIR)/utils.c
	$(CC) $(CFLAGS) -c $(SDIR)/utils.c -o $(ODIR)/utils.o

$(ODIR)/errors.o: $(SDIR)/errors.c
	$(CC) $(CFLAGS) -c $(SDIR)/errors.c -o $(ODIR)/errors.o

$(ODIR)/path.o: $(SDIR)/path.c
	$(CC) $(CFLAGS) -c $(SDIR)/path.c -o $(ODIR)/path.o

$(ODIR)/proc.o: $(SDIR)/proc.c
	$(CC) $(CFLAGS) -c $(SDIR)/proc.c -o $(ODIR)/proc.o

$(ODIR)/prompt.o: $(SDIR)/prompt.c
	$(CC) $(CFLAGS) -c $(SDIR)/prompt.c -o $(ODIR)/prompt.o

clean:
	rm -rf ./build

$(shell mkdir -p $(BDIR) $(ODIR))
