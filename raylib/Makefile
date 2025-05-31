CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lraylib -lm

# macOS specific flags
ifeq ($(shell uname), Darwin)
    CFLAGS += -I/opt/homebrew/include
    LIBS += -L/opt/homebrew/lib -framework OpenGL -framework IOKit -framework Cocoa
endif

# Linux specific flags  
ifeq ($(shell uname), Linux)
    LIBS += -lGL -lX11 -lpthread -ldl
endif

TARGET = block_breaker
SOURCE = main.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run