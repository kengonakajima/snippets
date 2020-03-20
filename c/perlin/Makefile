all:
	rm -f hoge.png
	gcc hoge.c
	./a.out
	md5 hoge.rgb
	convert -size 1024x1024 -depth 8 hoge.rgb hoge.png
