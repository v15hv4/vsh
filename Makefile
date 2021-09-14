IDIR=include
SDIR=src
BDIR=build
ODIR=build/obj

CC=gcc
CFLAGS=-I$(IDIR)

DEPS=vsh utils errors path proc prompt cd
OBJS=$(patsubst %, $(ODIR)/%.o, $(DEPS))

vsh: $(OBJS)
	$(CC) $(CFLAGS) -g $(OBJS) -o $(BDIR)/vsh 
	chmod +x build/vsh

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf ./build

$(shell mkdir -p $(BDIR) $(ODIR))
