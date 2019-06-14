ARCH := $(shell root-config --arch)
SHELL = /bin/sh
TOPDIR = ..
DEPDIR = .deps

ROOTLIBS     := $(shell root-config --glibs)
DIMLIBS  := -L$(DIMDIR)/$(ARCH) -ldim
ALIDCSLIBS := -L$(ALIDCSCLIENT) -lAliDCSClient
LIBS          =  $(ALIDCSLIBS) $(ROOTLIBS) -lMinuit -lGeom -lThread $(DIMLIBS)

OS_TYPE := $(subst -,,$(shell uname))
OS_VERSION := $(subst -,,$(shell uname -r))
MYOS := $(OS_TYPE)$(OS_VERSION)


INCLUDEROOT :=  $(shell root-config --incdir)
INCLUDEDIM := $(DIMDIR)/dim
INCLUDEALIDCS := $(ALIDCSCLIENT)
DEFAULT_INCLUDES =  -I. 
INCLUDES = $(DEFAULT_INCLUDES) -I$(INCLUDEROOT) -I$(INCLUDEDIM) -I$(INCLUDEALIDCS)

CC=g++
CFLAGS=-c
OFLAGS=-o
RM=rm -f
OPTIM=-O2 -bind_at_load

LDFLAGS = -O
ifeq ($(ARCH),linux)
SOFLAGS = -shared
OPTIM=-O0 
endif
ifeq ($(ARCH),macosx)
SOFLAGS = -dynamiclib
OPTIM=-O0 -bind_at_load
endif

CXXFLAGS =  $(OPTIM)

SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
DEP= $(SRC:.cpp=.d)
RACINE= $(SRC:.cpp=.)

BINDIR = $(GMSROOT)/bin

depcomp = $(SHELL) $(TOPDIR)/depcomp

CXXCOMPILE = $(CC) $(INCLUDES) $(CXXFLAGS)

all: depend2 GMSAnalyser

%.d:%.cpp
	@echo "Creating dependency file $@ ..."
#	@$(CC) $(INCLUDES) -M $< > $@ 
	@$(CC) $(INCLUDES) -M $< >> Makefile
	 

	
GMSAnalyser:  Makefile $(OBJ)
	@echo '***********************  Programme ' $@ ':************************' ;
	-rm -f $(BINDIR)/$@
	$(CC)  $(OPTIM) -g3 $(OFLAGS) $(BINDIR)/$@ $(OBJ) $(LIBS) 


.cpp.o:
	@echo "Compiling $@ ..."
	@$(CXXCOMPILE) -g3 -c -o $@ $<

libGMSAnalyser.so: $(OBJECTBATCH)
	@echo "Creating $@ ..."
	$(CXX) $(SOFLAGS)  $(LDFLAGS) $^ -o $@ 


# Plane.o: Plane.h Plane.cpp PlaneDict.cpp
# 	@echo "Compiling $@ ..."
# 	@$(CXX) $(INCLUDES) $(CXXFLAGS) -c Plane.cpp -o $@
# PlaneDict.o: PlaneDict.cpp
# 	@echo "Compiling dictionary $@ ..."
# 	@$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@
# PlaneDict.cpp: Plane.h PlaneLinkDef.h
# 	@echo "Generating dictionary $@ ..."
# 	@rootcint -f $@ -c -I. $^
# libPlane.so: Plane.o PlaneDict.o
# 	@echo "Creating $@ ..."
# 	@$(CXX) $(SOFLAGS)  $(LDFLAGS) $^ -o $@ 

# Chamber.o: Chamber.h Chamber.cpp ChamberDict.cpp
# 	@echo "Compiling $@ ..."
# 	@$(CXX) $(INCLUDES) $(CXXFLAGS) -c Chamber.cpp -o $@
# ChamberDict.o: ChamberDict.cpp
# 	@echo "Compiling dictionary $@ ..."
# 	@$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@
# ChamberDict.cpp: Chamber.h ChamberLinkDef.h
# 	@echo "Generating dictionary $@ ..."
# 	@rootcint -f $@ -c -I. $^
# libChamber.so: Chamber.o ChamberDict.o
# 	@echo "Creating $@ ..."
# 	@$(CXX) $(SOFLAGS)  $(LDFLAGS) $^ -o $@ 
#
#Wall.o: Wall.h Wall.cpp WallDict.cpp
#	@echo "Compiling $@ ..."
#	@$(CXX) $(INCLUDES) $(CXXFLAGS) -c Wall.cpp
#WallDict.o: WallDict.cpp
#	@echo "Compiling dictionary $@ ..."
#	@$(CXX) $(INCLUDES) $(CXXFLAGS) -c $<
#WallDict.cpp: Wall.h WallLinkDef.h
#	@echo "Generating dictionary $@ ..."
#	@rootcint -f $@ -c -I. $^


dir:
	@for dir in $(ALLOBJECTDIR) ; do if [ ! -d $${dir} ] ; then  mkdir $${dir} ;\
	echo "Making directory $${dir}" ; fi ;  done; 

clean: 
	-rm -f core*
	-ln -s /dev/null core
	-rm -rf *.o
	-rm -rf *.d

destroy: clean
	-rm -f src/GMSAnalyzer
	-rm -f *Dict*
	
depend:
	makedepend -I$(INCLUDES) $(SRC)

depend2:
	@rm -f Makefile.deps
	@touch Makefile.deps
	@for file in $(SRC) ; do $(CC) $(INCLUDES) -M $${file} >> Makefile.deps;\
	echo "Creating Dependency for  $${file}" ;  done; 

include Makefile.deps
