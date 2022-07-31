#!/bin/sh

# use this script to build

name=repr
cc=gcc

# change '.c' with '.o'
ctoo() {
	name="$1"
	echo $name | sed 's/.c$/.o/'
}

compile() {
	infile=$1
	outfile=`ctoo $infile`
	echo "compiling $infile -> $outfile"
	mkdir -p obj
	$cc -Wall -Wpedantic -ansi $dbg_flag -o obj/$outfile $infile
}

obj_files=""

if [ "$1" -eq "dbg" ]
then
	dbg_flag="-g"
fi

for file in .
do
	# file ends in ".c"? if so, compile it
	if [ "$file" -eq "*.c" ]
	then
		compile $file
		obj_files=$obj_files" `ctoo $file`"
	fi

	# remove first space
	obj_files=`echo $obj_files | sed 's/^ //'`

	$cc -o $name $obj_files
done
