all: fir echo dft

fir: fir.cpp
	g++ fir.cpp -o fir

echo: echo.cpp util.cpp
	g++ echo.cpp util.cpp -o echo

rec.png: echo
	./rw
	node wave.js rec.raw rec.png
	node wave.js ref.raw ref.png
	node wave.js final.raw final.png

dft: dft.cpp util.cpp
	g++ dft.cpp util.cpp -o dft

sine.png: dft
	node wave.js sine.raw sine.png
