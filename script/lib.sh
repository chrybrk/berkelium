./bin/c/bk $1 -S -o ./bin/bk/lib
gcc ./lib/*.c ./bin/bk/lib.s -o ./bin/c/libs

if [[ $? -eq 0 ]]; then
    ./bin/c/libs
fi
