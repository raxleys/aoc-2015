CC=gcc

main:
	mkdir -p build
	$(CC) -o build/main src/main.c -Iinc -Wall -Wextra -pedantic

clean:
	rm -rf build

.PHONY: clean
