IDIR=include
SDIR=src
BDIR=build
ODIR=build/obj

CC=gcc
CFLAGS=-I$(IDIR)
LIBS=-lm

DEPS=vsh utils errors path proc prompt builtins history ls pinfo signals terminal
OBJS=$(patsubst %, $(ODIR)/%.o, $(DEPS))

vsh: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -g $(OBJS) -o $(BDIR)/vsh 
	chmod +x build/vsh

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

clean:
	rm -rf ./build

$(shell mkdir -p $(BDIR) $(ODIR))
