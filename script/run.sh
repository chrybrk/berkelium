project_name="bk"
project_bin_dir="./bin/c/"
project_run_command="./bin/c/bk $1 -b ./lib/lib.c -o ./bin/bk/main"

compile_info="project"


if [[ $compile_info -eq "project" ]]; then
    gcc src/*.c -Isrc/include -o $project_bin_dir$project_name
    if [[ $? -eq 0 ]]; then
        $project_run_command
        if [[ $? -eq 0 ]]; then
            ./bin/bk/main
        fi
    else
        echo "program failed to compile, exited($?)"
    fi
elif [[ $compile_info -eq "testing/asm" ]]; then
    nasm -f elf $compile_info/main.asm -o ./bin/asm/main.o
    gcc -no-pie -m32 ./bin/asm/main.o -o ./bin/asm/main
    if [[ $? -eq 0 ]]; then
        ./bin/asm/main
    fi
elif [[ $compile_info -eq "testing/c" ]]; then
    gcc ./testing/c/*.c -I./testing/c/ -o $./bin/c/main
    if [[ $? -eq 0 ]]; then
        ./bin/c/main
    fi
fi


status=$?

if [[ $status -eq 0 ]]; then
    echo -e "\n"
    echo "project: compiled"
else
    echo -e "\n"
    echo "project: failed, result: $status"
fi
