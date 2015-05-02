all:
	g++ src/main.cpp src/gtia.cpp 6502Core/m6502.cpp -o atariemu -g -std=gnu++11
