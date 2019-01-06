SHELL = /bin/sh
CXX = clang++-4.0
CXXSTD = -std=c++1z -stdlib=libc++
WARNINGS = -pedantic -Wall -Werror -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-gnu -Wno-tautological-pointer-compare
CXXFLAGS = $(CXXSTD) -O3 $(WARNINGS)
LDFLAGS = $(CXXSTD)
EXE = hotmake

OBJS = MakeRecord.o inotify-cxx/inotify-cxx.o
SRC = $(EXE).cpp inotify-cxx/inotify-cxx.cpp

# Compile all dependencies
all : $(EXE)

# Compile hotmake main executable
$(EXE) : $(SRC) $(OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $@

# Clean up working directory
clean : 
	rm -rf $(EXE) $(OBJS)

.PHONY : clean all hotmake