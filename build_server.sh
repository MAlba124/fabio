#!/bin/sh

TOML_HPP_PATH="./server/include/toml.hpp"

if [ -e  $TOML_HPP_PATH ]
then
	printf "[✓ ] Toml++ found\n"
else
	printf "[✗ ] Toml++ was not found, downloading...\n"
	wget https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp -O $TOML_HPP_PATH
fi

printf "Running CMake...\n"

if [ -e "server/build" ]
then
	cd server/build
else
	mkdir server/build && cd server/build
fi

cmake ..

printf "Compiling...\n"

make -j4
