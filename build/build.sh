#/bin/env sh
########################################
# build script
########################################

cd `dirname "$0"`

if test -r Makefile ; then
	echo "Cleaning..."
	make clean
fi

which cmake
if test ! $? ; then
	echo "Error: cmake isn't installed. pls install it."
	exit
fi

if test ! -e libasar.so ; then
	echo "Error: \"libasar.so\" is minnsing..."
	exit
fi

rm -rf CMake* cmake_* bolib Makefile

echo "Build start..."
cmake ..; cmake ..; make
sync

