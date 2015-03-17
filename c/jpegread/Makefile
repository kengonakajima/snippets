

all: cumino.o
	g++ -g hoge.cpp -I. -ljpeg cumino.o -lz


cumino.o:cumino.cpp
	g++ -g -c cumino.cpp

clean:
	rm cumino.o a.out
