# use
# tail -f ~/Library/Preferences/Macromedia/Flash\ Player/Logs/flashlog.txt 
# for debugging

MXMLC=/Applications/Adobe\ Flex\ Builder\ 3/sdks/3.0.0/bin/mxmlc

all: build

build: main.swf

main.swf: main.as FlyingObject.as
	rm -f main.swf
	$(MXMLC) -default-size 500 400 -default-frame-rate 60 -debug main.as | nkf -w 2>&1

clean:
	rm *.swf

run:
	open main.swf

tail:
	tail -f ~/Library/Preferences/Macromedia/Flash\ Player/Logs/flashlog.txt


help:
	$(MXMLC) --help
