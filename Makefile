build:
	gcc -Isrc/include src/*.c -o bin/c/bk

run:
	./script/test.sh

program:
	./script/program_test.sh

install:
	sudo cp ./bin/c/bk /opt/
