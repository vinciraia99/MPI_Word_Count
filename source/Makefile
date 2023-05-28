CFLAGS = $(shell pkg-config --cflags glib-2.0)
CLIBS = $(shell pkg-config --libs glib-2.0)

LIBDIR = ../libraries
CXXFLAGS += -I$(LIBDIR)
IMPLEMENTATIONDIR = ../libraries/implementation

all : main.o file_info.o chunk.o log.o lexer.o word.o
	mpicc main.o chunk.o file_info.o log.o lexer.o word.o -o wordCount.out $(CFLAGS) $(CLIBS)
	rm -f *.o

main.o :
	mpicc $(CXXFLAGS) -c main.c $(CFLAGS) $(CLIBS)

log.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/log.c	$(CFLAGS) $(CLIBS)

lexer.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/lexer.c	$(CFLAGS) $(CLIBS)
	
file_info.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/file_info.c $(CFLAGS) $(CLIBS)

chunk.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/chunk.c

word.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/word.c	
	
clean: 
	rm -f *.out *.o