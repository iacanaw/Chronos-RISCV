
GCC=g++
EIGH=./eigen-eigen-b3f3d4950030


all: matex int

matex:
	$(GCC) -I $(EIGH) matex.cpp configurationParameters.cpp floorplan.cpp rcModel.cpp -o MatEx

int:
	$(GCC) matex_int.cpp -o matex_int

clean:
	rm -rf MatEx
