
#LIB=/Users/ringo/tmp/pb/protobuf-2.5.0/src/.libs/libprotobuf.a

LIB=-lprotobuf


all: pbtest



pbtest: game.pb.o pbtest.o
	g++  game.pb.o pbtest.o $(LIB)  $(LIBPROTOBUF) -o pbtest 

pbtest.o: pbtest.cc
	g++ -c -O1 pbtest.cc -o pbtest.o

game.pb.o: game.pb.cc
	g++ -c -O1 game.pb.cc

game.pb.cc game.pb.h:
	protoc game.proto --cpp_out=.


