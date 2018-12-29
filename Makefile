# Compile main executable
hotmake : hotmake.cpp
	clang++ hotmake.cpp -o hotmake

# Clean up working directory
clean : 
	rm -rf ./hotmake