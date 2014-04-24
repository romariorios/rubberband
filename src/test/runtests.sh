#!/bin/sh

if [ -z $CXX ]
then
    echo "No CXX defined"
    exit -1
fi

for t in *test.cpp
do
    test_name=${t%\.*}-$CXX
    echo "--- $test_name ---"
    $CXX $t ../core/object.cpp ../core/symbol.cpp ../core/block.cpp -o $test_name $CXX_FLAGS &&
    ./$test_name &&
    rm $test_name
done

