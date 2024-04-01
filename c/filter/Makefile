all: fir echo no_echo.png

fir: fir.cpp
	g++ fir.cpp -o fir

echo: echo.cpp
	g++ echo.cpp -o rw

no_echo.png: echo
	./rw
	node wave.js no_echo.raw no_echo.png
	node wave.js with_echo.raw with_echo.png
