if [ $1 -eq "0" ]; then
    ./script/test.sh | gnomon --medium=1.0 --high=4.0 --ignore-blank --real-time=100
else
    ./script/program_test.sh | gnomon --medium=1.0 --high=4.0 --ignore-blank --real-time=100
fi
echo "all test cases are completed"
