SHELL = /bin/sh
CXX = clang++-3.8
CXXSTD = -std=c++1z -stdlib=libc++
WARNINGS = -pedantic -Wall -Werror -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS = $(CXXSTD) -O3 $(WARNINGS)
LDFLAGS = $(CXXSTD)
EXE = hotmake

OBJS = 

# Compile all dependencies
all : $(EXE)

# Compile hotmake main executable
$(EXE) : $(EXE).cpp $(OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $@

# Clean up working directory
clean : 
	rm -rf $(EXE) $(OBJS)

.PHONY : clean all