TARGET = convert

all: 
	g++ main.cpp -std=c++17 $(shell root-config --cflags --glibs) -lcapnp -lkj -o $(TARGET)  