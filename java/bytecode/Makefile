all:
	rm *.class
	javac Aho.java
	javap -c Aho Hoge > a.S
	mcs Aho.cs
	monodis Aho.exe > monoa.S
	
	