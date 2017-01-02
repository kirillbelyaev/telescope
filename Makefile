EXEC      = telescope 
CC = gcc
CFLAGS = -Wall -fno-strict-aliasing -g -I/usr/include/libxml2  -DHAVE_CONFIG_H
LDFLAGS = -lxml2 -lpthread -lcrypt -lm -lz

SOURCE := telescope.c \
          functions.c \
          cli.c \
          queuelib.c \
          status.c \
          parserEngine.c \
          fileReader.c \
          connectionManager.c \
          passwordManager.c \
          server.c \
          subscriber.c \

AR = @AR@
ARFLAGS = @ARFLAGS@
RANLIB = @RANLIB@

INSTALL_MODE= -m 0755

# Include some boilerplate Gnu makefile definitions.
prefix = /usr/local

exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
infodir = ${prefix}/share/info
includedir = ${prefix}/include
datadir = ${prefix}/share
docdir = $(datadir)/doc/telescope
localedir = $(datadir)/locale

mandir = ${prefix}/share/man
manpfx = man
man1ext = .1
man1dir = $(mandir)/$(manpfx)1man3ext = .3man3dir = $(mandir)/$(manpfx)3

htmldir = ${docdir}

SRCDIR = ./src
BINDIR = ./bin
OBJDIR = ./Obj
OBJECTS  = $(foreach var,$(SOURCE),$(OBJDIR)/$(var:.c=.o))
MAINEXEC  = $(BINDIR)/telescope

.PHONY: directories

all: directories $(EXEC)

$(EXEC): $(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS)  $(LDFLAGS) -o $(MAINEXEC)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -f $(EXEC) $(MAINEXEC) $(OBJECTS)  	

directories:
	mkdir -p Obj bin

# build tests
build-tests: .build-tests-post

.build-tests-pre:
# Add your pre 'build-tests' code here...

.build-tests-post: .build-tests-impl
# Add your post 'build-tests' code here...


# run tests
test: .test-post

.test-pre:
# Add your pre 'test' code here...

.test-post: .test-impl
# Add your post 'test' code here...
