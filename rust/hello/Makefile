all: main cpp buf
	time ./main
	time ./buf
	time ./a.out

buf: buf.rs
	rustc buf.rs
main: main.rs
	rustc main.rs
cpp: hoge.cpp
	g++ hoge.cpp
