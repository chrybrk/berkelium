test_dir="./test/"
files=`ls $test_dir*.bk`

for file in $files
do
    ./bin/c/bk $file -o ./bin/bk/test_case > ./bin/bk/err.txt
    if [[ $? -eq 0 ]]; then
        echo "test_case: pass"
    else
        echo "test_case: failed"
    fi
done

echo "all test cases are completed"
