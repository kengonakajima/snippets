all: f.js g.js

f.js: hoge.c
	emcc hoge.c -o f.js -s EXPORTED_FUNCTIONS="['_hoge','_lll']"
g.js: poo.cpp
	emcc poo.cpp -o g.js -s EXPORTED_FUNCTIONS="['_poo','_zzz','_iii','_ccc','_sss','_sizeofsizet']"

test: f.js g.js
	node test.js
