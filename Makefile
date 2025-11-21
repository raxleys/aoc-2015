CC=gcc

main:
	mkdir -p build
	$(CC) -o build/main src/main.c -Iinc -Wall -Wextra -pedantic

test:
	mkdir -p build
	$(CC) -o build/main src/main.c -Iinc -DTEST -Wall -Wextra -pedantic

clean:
	rm -rf build

.PHONY: clean
