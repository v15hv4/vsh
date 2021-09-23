IDIR=include
SDIR=src
BDIR=build
ODIR=build/obj
CDIR=build/commands

CC=gcc
CFLAGS=-I$(IDIR)
LIBS=-lm

_CMDS=ls pinfo jobs
CMDS=$(patsubst %, $(CDIR)/%.o, $(_CMDS))

_DEPS=vsh utils errors path proc prompt builtins history signals terminal
DEPS=$(patsubst %, $(ODIR)/%.o, $(_DEPS))

vsh: $(DEPS) $(CMDS)
	$(CC) $(CFLAGS) $(LIBS) -g $(DEPS) $(CMDS) -o $(BDIR)/vsh 
	chmod +x build/vsh

$(CDIR)/%.o: $(SDIR)/commands/%.c
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

clean:
	rm -rf ./build

$(shell mkdir -p $(BDIR) $(ODIR) $(CDIR))
