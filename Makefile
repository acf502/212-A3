# Name: Aaron Colin Foote
# Date: 20th May 2013
# User: acf502 / 4258770
# File.: server.cpp for CSCI212 Assignment 3
SVR_OBJECT_FILES = server.o wrappers.o
CLT_OBJECT_FILES = client.o wrappers.o 
TARGETS=server.o client.o wrappers.o
CC=CC
INCLUDES=
LIBS=-lsocket -lnsl -lresolv
CFLAGS=-g $(INCLUDES)

all: client server
client: $(CLT_OBJECT_FILES)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
server: $(SVR_OBJECT_FILES)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


client.o server.o wrappers.o: wrappers.h

%.o: %.cpp
	$(CC) -c $<

clean:
	rm -f *.o $(TARGETS)

