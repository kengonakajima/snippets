
rec: rec.cpp
	g++ rec.cpp -o rec ../../../portaudio/lib/.libs/libportaudio.a -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework CoreFoundation

devs: pa_devs.c
	gcc pa_devs.c -o devs ../../../portaudio/lib/.libs/libportaudio.a -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework CoreFoundation

a.out: hoge.cpp
	g++ hoge.cpp -o a.out ../../../portaudio/lib/.libs/libportaudio.a -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework CoreFoundation
