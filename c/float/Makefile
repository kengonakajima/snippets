all: 2d 2di

2d: 2d.c
	gcc -O3 2d.c -o 2d

2di: 2di.c
	gcc -O3 2di.c -o 2di

conv:
	cd out; rm *png *.mp4; ruby x.rb; ffmpeg -v debug -r 30 -i hoge_%03d.png -vcodec libx264 -pix_fmt yuv420p -r 60 out.mp4     
