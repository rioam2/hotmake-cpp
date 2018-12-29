# Compile main executable
hotmake : main.cpp
	clang++ main.cpp -o hotmake

# Clean up working directory
clean : 
	rm -rf ./hotmake