a:
	gcc poo.c simplexnoise1234.c sdnoise1234.c srdnoise23.c
	./a.out
	convert -size 1024x1024 -depth 8 hoge.rgb hoge.png

t:
	rm -f hoge.png
	gcc hoge.c
	./a.out
	md5 hoge.rgb
	convert -size 1024x1024 -depth 8 hoge.rgb hoge.png

