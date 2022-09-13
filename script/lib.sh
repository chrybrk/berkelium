./bin/c/bk $1 -b ./lib/lib.c -o ./bin/bk/lib

if [[ $? -eq 0 ]]; then
    ./bin/c/libs
fi
