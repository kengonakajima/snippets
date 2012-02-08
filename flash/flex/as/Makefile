MXMLC=/Applications/Adobe\ Flex\ Builder\ 3/sdks/3.0.0/bin/mxmlc

all: build

build: main.swf

main.swf: main.as
	$(MXMLC) main.as | nkf -e 2>&1

clean:
	rm *~ *.swf

run:
	open main.swf


help:
	$(MXMLC) --help
