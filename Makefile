executable:=./src/main

build:=./build
src:=./src

objects:=$(build)/archivo.o
sources:=$(src)/archivo.cpp

cxx:=g++

includes:=-I./src/include 

main: $(objects)
	$(cxx) $(includes) $(objects) -o $(build)/laboratorio2 $(executable).cpp

$(build)/%.o: $(src)/%.cpp 
	test -d $(build) || mkdir $(build)
	$(cxx) $(includes) -c -o $(build)/$(*).o $(src)/$*.cpp

clean:
	rm $(build)/*;


	
