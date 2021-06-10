#!/bin/bash
ALGORITHM=$1

FILES=(
	"../Makefile"
	"../inc/ft_cmd.h"
	"../ft_ssl"
	"test/zero.txt"
	"test/empty.txt"
	"test/lazydog.txt"
#	"`which vim`"
	)

cypher_test()
{
	arr=("$@")

#	for i in "${arr[@]}"; do
#		printf "\t%s\n" $i
#		./cypher_key.sh $ALGORITHM $i
#	done
	for i in "${arr[@]}"; do
		printf "\t%s\n" $i
		./cypher_pwd.sh $ALGORITHM $i
	done
}

cypher_test "${FILES[@]}"
