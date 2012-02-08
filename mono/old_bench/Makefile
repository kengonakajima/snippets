all:gene gene.exe RegexBench2.class r bug

RegexBench2.class: RegexBench2.java
	javac RegexBench2.java

gene.exe: gene.cs
	mcs gene.cs
	
gene: gene.cpp
	g++ -c -I/usr/local/include/boost-1_33_1 gene.cpp -o gene.o
	g++ gene.o /usr/local/lib/libboost_regex.a -o gene


clean:
	rm RegexBench2.class gene.exe gene

r: r.cpp
	g++ -c -I/usr/local/include/boost-1_33_1 r.cpp -o r.o
	g++ r.o /usr/local/lib/libboost_regex.a -o r
	
bug: bug.cpp
	g++ -c -I/usr/local/include/boost-1_33_1 bug.cpp -o bug.o
	g++ bug.o /usr/local/lib/libboost_regex.a -o bug