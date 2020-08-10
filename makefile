CC=g++
CFLAGS=-c -g -O0 -std=c++11 -Wall
LDFLAGS=-lpthread -lrt
SOURCES=serverA.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=serverA

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f core $(EXECUTABLE) $(OBJECTS)
