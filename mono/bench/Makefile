all: bench.rb bench bench.exe Bench.class iob Iob.class iob.exe bench-o bench-o.exe

Bench.class: Bench.java
	javac Bench.java

bench.exe: bench.cs
	mcs bench.cs
	
bench: bench.cpp
	g++ -c -I/usr/local/include/boost-1_33_1 bench.cpp -o bench.o
	g++ bench.o /usr/local/lib/libboost_regex.a -o bench

bench-o:
	g++ -O6 -c -I/usr/local/include/boost-1_33_1 bench.cpp -o bench.o
	g++ bench.o /usr/local/lib/libboost_regex.a -o bench-o

bench-o.exe:
	mcs -optimize+ bench.cs -out:bench-o.exe

iob:
	g++ -c iob.cpp -o iob

Iob.class: Iob.java
	javac Iob.java
			
iob.exe: iob.cs
	mcs iob.cs
				
clean:
	rm Bench.class bench.exe bench *.o

run:
	rm -f java_out mono_out cpp_out ruby_out
	time ./bench 1000 > cpp_out
	time mono bench.exe 1000 > mono_out
	time java Bench 1000 > java_out	
	time ruby bench.rb 1000 > ruby_out
	wc java_out
	diff java_out cpp_out
	diff mono_out cpp_out
	diff ruby_out cpp_out
	rm -f java_out mono_out cpp_out ruby_out
	time ./bench 15000 > cpp_out
	time mono bench.exe 15000 > mono_out
	time java Bench 15000 > java_out
	wc java_out
	diff java_out cpp_out
	diff mono_out cpp_out
	time ./iob 5000000 > cpp_out
	time mono iob.exe 5000000 > mono_out
	time java Iob 5000000 > java_out
	time ruby iob.rb 5000000 > ruby_out
	wc java_out
	diff java_out cpp_out
	diff mono_out cpp_out
	diff ruby_out cpp_out
	rm -f java_out mono_out cpp_out


	
	