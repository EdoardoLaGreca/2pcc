#!/bin/sh

# use this script to build

cc=gcc

compile() {
	infile=$1
	outfile=`echo $file | sed "s/.c$/.o/"`
	echo "compiling $infile -> $outfile"
	mkdir -p obj
	$cc -Wall -Wpedantic -ansi -o obj/$outfile $infile
}

compile_dbg() {
	infile=$1
	outfile=`echo $file | sed "s/.c$/.o/"`
	echo "compiling (debug) $infile -> $outfile"
	$cc -Wall -Wpedantic -ansi -g -o obj/$outfile $infile
}

for file in .
do
	# file ends in ".c"? if so, compile it
	if [ "$file" -eq "*.c" ]
	then
		if [ "$1" -eq "dbg" ]
		then
			compile_dbg $file
		else
			compile $file
		fi
	fi
	
done
