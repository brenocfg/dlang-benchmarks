LLVM_PATH=$HOME/Work/llvm601-build/bin

FILES=$(find ./progs/ -name "*.d")

for f in $FILES
do
	echo $f
	sudo ldc2 -output-ll $f
done

BCFILES=$(find . -name "*.ll")

for f in $BCFILES
do
	$LLVM_PATH/opt  -load DCCBasilisk.dylib -Instrument -debug-only=Instrument $f -S -o $f
	$LLVM_PATH/opt t1.rbc -o t1.o
	#$LLVM_PATH/clang ../Collect/collect.o t1.o -O2 -o t1.exe
done 
