CC=gcc
ARGS=-Wall -Wextra -Wpedantic -g
LINKS=-fopenmp -lm
OUTPUT=ecosystem

build:
	$(CC) $(ARGS) src/*.c -o $(OUTPUT) $(LINKS)

time:
	$(CC) $(ARGS) src/*.c -o $(OUTPUT) $(LINKS) -D TIME

clean:
	rm -rf $(OUTPUT)