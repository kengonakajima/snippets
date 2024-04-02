all: fir echo rec.png

fir: fir.cpp
	g++ fir.cpp -o fir

echo: echo.cpp
	g++ echo.cpp -o rw

rec.png: echo
	./rw
	node wave.js rec.raw rec.png
	node wave.js ref.raw ref.png
	node wave.js final.raw final.png
