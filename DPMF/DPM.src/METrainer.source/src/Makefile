CXX  	    = $(CXX)
CXXFLAGS    = $(CXXFLAGS)
LINKFLAGS   = $(LINKFLAGS)
LIBFLAGS    = $(LIBFLAGS)
LIBDIR      = $(LIBDIR)

PROGRAM     = beam_sr_trainer

SRC         = Main.cpp  Trainer.cpp  Simulator.cpp  DepTree.cpp
OBJ         = Main.o    Trainer.o    Simulator.o    DepTree.o
.SUFFIXES:
.SUFFIXES: .o .c .cpp

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $*.cpp

all: $(PROGRAM)

$(PROGRAM): $(OBJ) 
	$(CXX) $(LINKFLAGS) $(LIBFLAGS) -o $(PROGRAM) $(OBJ) $(ASM)

clean:
	-rm -f *.o $(PROGRAM)
