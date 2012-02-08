all: teste test.exe test-invoke

teste: teste.c 
	gcc `pkg-config --cflags --libs mono` -lm teste.c -o teste
	
run:
	./teste test.exe
	
test.exe: test.cs
	mcs test.cs
	
	
test-invoke: test-invoke.c
	gcc `pkg-config --cflags --libs mono` -lm test-invoke.c -o test-invoke

runi:
	./test-invoke invoke.exe

invoke.exe: invoke.cs
	mcs invoke.cs
	
		
clean:
	rm test.exe teste
	
	
lsbom:
	lsbom /Library/Receipts/MonoFramework-1.1.17.2_0.macos10.novell.x86.pkg/Contents/Archive.bom
	