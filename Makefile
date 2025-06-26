# Makefile for Space Invaders
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = $(shell pkg-config --libs allegro-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5)
CFLAGS += $(shell pkg-config --cflags allegro-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5)

TARGET = space_invaders
SOURCE = space_invaders.c

# Default target
all: $(TARGET)

# Compile the game
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

# Clean build files
clean:
	rm -f $(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y liballegro5-dev liballegro5.2 liballegro-acodec5-dev liballegro-audio5-dev liballegro-dialog5-dev liballegro-image5-dev liballegro-physfs5-dev liballegro-ttf5-dev liballegro-video5-dev liballegro-primitives5-dev

# Run the game
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean install-deps run 