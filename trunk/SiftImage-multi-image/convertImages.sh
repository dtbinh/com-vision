#!/bin/sh

if [ -d "$1" ]; then
    for file_name in $1/*
    do
        path_list=$(echo $file_name | tr "." "\n");
	l=0;
	for x in $path_list
	do
	    path_array[$l]=$x;
	    l=$l+1;
	done

	name_list=$(echo ${path_array[0]} | tr "/" "\n");
	l=0;
	for x in $name_list
	do
	    array[$l]=$x;
	    l=$l+1;
	done
        convert $file_name pgm/${array[$l-1]}".pgm";
    done
fi;
