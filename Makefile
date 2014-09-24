cxx:=g++
salida:= laboratorio2


programa: main.o archivo.o
	$(cxx) -I./src/include -o ./build/laboratorio2 *.o

main.o: src/main.cpp src/include/bibliotecas.h
	$(cxx) -I./src/include -c src/main.cpp

archivo.o: src/archivo.cpp	src/include/bibliotecas.h
	$(cxx) -I./src/include -c src/archivo.cpp


clean:
	rm *.o
	rm ./build/laboratorio2
	
	
