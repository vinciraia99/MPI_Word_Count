CFLAGS = $(shell pkg-config --cflags glib-2.0)
CLIBS = $(shell pkg-config --libs glib-2.0)

LIBDIR = ../libraries
CXXFLAGS += -I$(LIBDIR)
IMPLEMENTATIONDIR = ../libraries/implementation

all : main.o file_info.o mpi_chunk.o log.o wordlist.o word.o
	mpicc main.o mpi_chunk.o file_info.o log.o wordlist.o word.o -o word_count.out $(CFLAGS) $(CLIBS)

main.o :
	mpicc $(CXXFLAGS) -c main.c $(CFLAGS) $(CLIBS)

log.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/log.c	$(CFLAGS) $(CLIBS)

wordlist.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/wordlist.c	$(CFLAGS) $(CLIBS)
	
file_info.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/file_info.c $(CFLAGS) $(CLIBS)

mpi_chunk.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/mpi_chunk.c

word.o:
	mpicc $(CXXFLAGS) -c $(IMPLEMENTATIONDIR)/word.c	
	
clean: 
	rm -f *.out *.o

clean_o:
	rm -f *.o