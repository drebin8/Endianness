# The variable CC specifies which compiler will be used.
# (because different unix systems may use different compilers)
CC=gcc

# The variable CFLAGS specifies compiler options
#   -c :    Only compile (don't link)
#   -Wall:  Enable all warnings about lazy / dangerous C programming 
#   -std=c99: Using newer C99 version of C programming language
#   -01:    Optimization level 1
CFLAGS=-c -Wall -std=c99 -O1

# All of the .h header files to use as dependencies
READ_HEADERS=endian_reader.h
WRITE_HEADERS=endian_writer.h

# All of the object files to produce as intermediary work
READ_OBJECTS=endian_reader.o
WRITE_OBJECTS=endian_writer.o

# The final program to build
EXECUTABLES=endian_reader_program endian_writer_program

# --------------------------------------------

all: $(EXECUTABLES)

endian_reader_program : $(READ_OBJECTS)
	$(CC) $(READ_OBJECTS) -o endian_reader_program

endian_writer_program : $(WRITE_OBJECTS)
	$(CC) $(WRITE_OBJECTS) -o endian_writer_program

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o $(EXECUTABLES)
