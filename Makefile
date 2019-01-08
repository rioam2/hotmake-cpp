SHELL = /bin/sh
CXX = clang++-4.0
CXXSTD = -std=c++1z -stdlib=libc++
WARNINGS = -pedantic -Wall -Werror -Wfatal-errors -Wextra -Wno-unused-parameter \
		   -Wno-unused-variable -Wno-gnu -Wno-tautological-pointer-compare
CXXFLAGS = $(CXXSTD) -O3 $(WARNINGS)
LDFLAGS = $(CXXSTD)
EXE = hotmake

OBJS = MakeRecord.o inotify-cxx/inotify-cxx.o

# Compile all dependencies
all : $(EXE)

# Compile hotmake main executable
$(EXE) : $(EXE).cpp $(OBJS)
	$(CXX) $^ $(CXXFLAGS) -lpthread -lm -o $@

# Clean up working directory
clean : 
	rm -rf $(EXE) $(OBJS)

.PHONY : clean all