#!/bin/bash

C_FILES="$(find rosetta -name "*.c")"
CPP_FILES="$(find rosetta -name "*.cpp")"
D_FILES="$(find rosetta -name "*.d")"

for file in $C_FILES; do
	filename="${file%.*}"
	filename="${filename}_c"
	gcc -O0 $file -o ${filename}
done

for file in $CPP_FILES; do
	filename="${file%.*}"
	filename="${filename}_cpp"
	g++ -O0 $file -std=c++14 -o ${filename}
done

for file in $D_FILES; do
	filename="${file%.*}"
	filename="${filename}_d"
	filename_bounds="${filename}_nobounds"
	ldc2 -O0 $file -of=${filename}
	ldc2 -O0 $file -boundscheck=off -of${filename_bounds}
done
