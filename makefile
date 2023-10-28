TARGET = convert

all:
	g++ main.cpp -std=c++17 -lcapnp -lkj -o $(TARGET)  