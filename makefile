TARGET = convert

all: 
	capnp compile -oc++ eventProto.capnp
	g++ main.cpp functions.cpp -std=c++17 $(shell root-config --cflags --glibs) -lcapnp -lkj -o $(TARGET)
