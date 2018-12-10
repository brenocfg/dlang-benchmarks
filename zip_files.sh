#!/bin/bash

C_FILES="$(find . -name "*.c")"
CPP_FILES="$(find . -name "*.cpp")"
D_FILES="$(find . -name "*.d")"

for file in $C_FILES; do
	ZIP_NAME="${file}.zip"
	zip $ZIP_NAME $file
done

for file in $CPP_FILES; do
	ZIP_NAME="${file}.zip"
	zip $ZIP_NAME $file
done

for file in $D_FILES; do
	ZIP_NAME="${file}.zip"
	zip $ZIP_NAME $file
done
