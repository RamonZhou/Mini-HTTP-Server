.PHONY: clean

CC = g++
CCFLAGS = -std=c++17 -Wall -lpthread -lm
SOURCE = main.cpp

all: $(SOURCE)
	$(CC) $(SOURCE) -o main $(CCFLAGS)

run: $(SOURCE)
	$(CC) $(SOURCE) -o main $(CCFLAGS)
	./main

debug: $(SOURCE)
	$(CC) $(SOURCE) -o main $(CCFLAGS) -DDEBUG

clean:
	rm main