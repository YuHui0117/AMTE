CXX  	    = $(CXX)
CXXFLAGS    = $(CXXFLAGS)
LINKFLAGS   = $(LINKFLAGS)
LIBFLAGS    = $(LIBFLAGS)
LIBDIR      = $(LIBDIR)

PROGRAM     = shift_reduce

SRC         = Main.cpp  maxent.cpp  Decoder.cpp  
OBJ         = Main.o    maxent.o    Decoder.o   
.SUFFIXES:
.SUFFIXES: .o .c .cpp

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $*.cpp

all: $(PROGRAM)

$(PROGRAM): $(OBJ) 
	$(CXX) $(LINKFLAGS) $(LIBFLAGS) -o $(PROGRAM) $(OBJ) $(ASM)

clean:
	-rm -f *.o $(PROGRAM)
