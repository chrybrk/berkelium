build:
	gcc -Isrc/include ./src/*.c -o brk

rebuild:
	rm brk
	gcc -Isrc/include ./src/*.c -o brk

run:
	./brk
