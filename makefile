# makefile for building the TCBVRP program

#DEBUG = -g
DEBUG = -O4

# cplex includes
SYSTEM     = x86-64_linux
LIBFORMAT 	= static_pic
CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio126/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio126/concert
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTINCDIR   = $(CONCERTDIR)/include
CPLEXINCDIR     = $(CPLEXDIR)/include

CCFLAGS += -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -DIL_STD -std=c++0x $(DEBUG) -Wall
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR)  -lconcert -lm -lpthread

EXE=tcbvrp
CPP=g++

SRCS=Main.cpp Instance.cpp tcbvrp_ILP.cpp Tools.cpp

OBJS=$(SRCS:.cpp=.o)

$(EXE): $(OBJS) 
	$(CPP) $(CCFLAGS) -o $(EXE) $(OBJS) $(CCLNFLAGS)

all: $(EXE)

clean:
	rm -f $(OBJS) $(EXE)

	
.SUFFIXES: .o .cpp
.cpp.o:
	$(CPP) -c $(CCFLAGS) $<
