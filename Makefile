CC=gcc

main:
	mkdir -p build
	$(CC) -o build/main src/main.c -Iinc -Wall -Wextra -pedantic -ggdb

test:
	mkdir -p build
	$(CC) -o build/main src/main.c -Iinc -DTEST -Wall -Wextra -pedantic -ggdb

clean:
	rm -rf build

.PHONY: clean
