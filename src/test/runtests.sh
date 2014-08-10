#!/bin/sh

if [ -z $CXX ]
then
    echo "No CXX defined"
    exit -1
fi

if [ -z $TESTS ]
then
    tests=$(echo *test.cpp)
else
    tests=$TESTS
fi

for t in $tests
do
    test_name=${t%\.*}-$CXX
    echo "--- $test_name ---"
    $CXX $t ../core/object.cpp ../core/symbol.cpp ../core/block.cpp -o $test_name $CXX_FLAGS -std=c++11 &&
    ./$test_name &&
    rm $test_name
done

