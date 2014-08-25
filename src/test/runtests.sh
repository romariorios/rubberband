#!/usr/bin/env bash

command -v cmake > /dev/null || {
    echo "cmake is not in PATH"
    exit -2
}

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

if [ -z $RUBBERBAND_SRC ]
then
    RUBBERBAND_SRC=$PWD/../core
fi

if [ -d build ]
then
    rm build -rf
fi &&

mkdir build

if [ ! -d inst ]
then
    mkdir inst
fi &&

cd build &&
    cmake $RUBBERBAND_SRC\
        -DCMAKE_INSTALL_PREFIX=../inst\
        -DCMAKE_CXX_COMPILER=$CXX\
        -DCMAKE_CXX_FLAGS=$CXX_FLAGS &&
    make install &&
cd .. &&

RUBBERBAND_PATH=$PWD/inst &&

for t in $tests
do
    test_name=${t%\.*}-$CXX
    echo "--- $test_name ---"
    $CXX\
        $t\
        -lrubberbandcore\
        -o $test_name\
        $CXX_FLAGS\
        -I$RUBBERBAND_PATH/include\
        -L$RUBBERBAND_PATH/lib\
        -std=c++11 &&
    LD_LIBRARY_PATH=$RUBBERBAND_PATH/lib ./$test_name &&
    rm $test_name
done


