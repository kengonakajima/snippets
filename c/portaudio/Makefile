PA=/opt/homebrew/Cellar/portaudio/19.7.0
PALIB=$(PA)/lib/libportaudio.a
PAINC=-I $(PA)/include
#ARCH=arch -x86_64
rec: rec.cpp
	$(ARCH) g++ rec.cpp -o rec $(PALIB) $(PAINC) -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework CoreFoundation -framework Carbon

devs: pa_devs.c
	$(ARCH) gcc pa_devs.c -o devs $(PALIB) $(PAINC) -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework CoreFoundation -framework Carbon

a.out: hoge.cpp
	$(ARCH) g++ hoge.cpp -o a.out $(PALIB) $(PAINC) -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework CoreFoundation -framework Carbon

