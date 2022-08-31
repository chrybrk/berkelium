gcc src/*.c -Isrc/include -o ./bin/c/bk

test_dir="./test/"
files=`ls $test_dir`

for file in $files
do
    ./bin/c/bk $test_dir$file -o ./bin/bk/test_case > ./bin/bk/err.txt
    if [[ $? -eq 0 ]]; then
        echo -e "[file: $file][status: pass]"
    else
        echo -e "[file: $file][status: failed]"
    fi
done

echo "all test cases are completed"
