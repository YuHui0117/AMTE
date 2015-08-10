CC=g++
CFLAGS=-c -Wall -g
LDFLAGS=
SOURCES=main.cpp  myalgorithm.cpp  parsedepfile.cpp  treealgorithm.cpp  treenode.cpp
OBJECTS=$(SOURCES:.cpp=.o)
	EXECUTABLE=score

all: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
	rm -fr *.o score 

