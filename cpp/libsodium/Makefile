all: kx

kx: kx.cpp
	g++ kx.cpp -I. libsodium.a -o kx
	./kx

test:
	./kx
