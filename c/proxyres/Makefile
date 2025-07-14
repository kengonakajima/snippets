CC = clang
CFLAGS = -Wall -Wextra -std=c99 -I/opt/homebrew/opt/duktape/include
LDFLAGS = -framework CoreFoundation -framework CFNetwork -framework SystemConfiguration -lcurl -L/opt/homebrew/opt/duktape/lib -lduktape

TARGET = proxy_detect
SOURCE = proxy_detect.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean