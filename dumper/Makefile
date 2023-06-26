CC = gcc
CFILE = dumper.c
DEP_CFILES = serial_linux.c
DEP_OBJECTS = serial_linux.o
DEP_HEADERS = serial_linux.h
FLAGS = -Wall -pedantic
DEBUG_FLAGS = -DDEBUG -Wextra

all: dumper

dumper: $(CFILE) $(DEP_OBJECTS) $(DEP_HEADERS)
	$(CC) $(FLAGS) $(CFILE) $(DEP_OBJECTS) -o dumper

debug: $(CFILE) $(DEP_OBJECTS) $(DEP_HEADERS)
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(CFILE) $(DEP_OBJECTS) -o dumper

serial_linux.o: serial_linux.c serial_linux.h
	$(CC) $(FLAGS) -c serial_linux.c -o serial_linux.o

clean:
	rm -f *.o dumper