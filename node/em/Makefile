all: hoge.js

hoge.js : hoge.c
	emcc -sWASM=1 -sEXPORTED_RUNTIME_METHODS='["cwrap"]'  -o hoge.js  hoge.c 

clean:
	rm hoge.js
    