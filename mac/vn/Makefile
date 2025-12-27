CC = clang
FRAMEWORKS = -framework Foundation -framework Vision -framework AppKit
CFLAGS = -fobjc-arc

ocr: main.m
	$(CC) $(CFLAGS) $(FRAMEWORKS) -o ocr main.m

clean:
	rm -f ocr

.PHONY: clean
