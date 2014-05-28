EXEC      = telescope 
CC = gcc
CFLAGS = -Wall -fno-strict-aliasing -g -I/usr/include/libxml2  -DHAVE_CONFIG_H
LDFLAGS = -lxml2 -lpthread -lm -lz


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
OBJECTDIR = ./Obj
MAINOBJS  = $(OBJECTDIR)/telescope.o
MAINEXEC  = $(BINDIR)/telescope

OBJECTS1 = $(MAINOBJS)

all: $(EXEC)

$(EXEC): $(OBJECTS1) 
	$(CC) $(CFLAGS) $(OBJECTS1) $(LDFLAGS) -o $(MAINEXEC)

$(OBJECTDIR)/telescope.o: $(SRCDIR)/telescope.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/telescope.c -o $(OBJECTDIR)/telescope.o


clean:
	rm -f $(EXEC) $(MAINEXEC) $(OBJECTS1)  	


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
