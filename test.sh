mkdir -p ./build

cc='clang++'
# flags='-std=c++11 -O0 -g -Wall -Wextra -pedantic -ferror-limit=3'
flags='-std=c++11 -O0 -g -Wall -Wextra -ferror-limit=3'
flags_pp='-E -DNO_STD_LIB'
flags_obj='-c'
flags_out=''

objs=''

for file in $(ls ./*.cpp)
do
    new=$($cc $flags_pp $flags $file)

    echo $new | diff -q - ./build/$file 1> /dev/null 2> /dev/null
    if [ $? -eq 0 ]
    then
        echo 'not changed: '$file
        echo
    else
        echo 'changed: '$file

        echo $cc $flags_obj $flags $file -o ./build/$file.o
        $cc $flags_obj $flags $file -o ./build/$file.o
        if [ $? -eq 0 ]
        then
            echo $new > ./build/$file
        else
            echo '' > ./build/$file
            exit
        fi
        echo
    fi

    objs=$(echo $objs ./build/$file.o)
done

cppcheck -q --std=c++11 --enable=all --inconclusive *.cpp
echo

echo $cc $flags_out $flags $objs -o ./build/test.out
$cc $flags_out $flags $objs -o ./build/test.out && ./build/test.out