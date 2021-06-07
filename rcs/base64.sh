#!/bin/bash

readlinkf(){ perl -MCwd -e 'print Cwd::abs_path shift' "$1";}

FILES=(
	"../Makefile"
	"../inc/ft_digest.h"
	"../ft_ssl"
	"test/zero.txt"
	"test/lazydog.txt"
	"test/empty.txt")
#	"`which vim`"
#	)

PROGRAM="../ft_ssl"

greentext()
{
	printf "\e[32m"
	printf "OK : %s\n" $1
	printf "\e[0m"
}

redtext()
{
	printf "\e[31m"
	printf "KO : %s\n" $1
	printf "\e[0m"
}

base64_file_reader()
{
	openssl base64 -e -in $1 > /tmp/shasum.txt
	$PROGRAM base64 -e -i $1 > /tmp/ft_ssl.txt
	arg=`diff /tmp/ft_ssl.txt /tmp/shasum.txt`
	printf "encrypt: "
	if [[ -z  "$arg" ]]; then
		greentext $1
	else
		redtext $1
	fi
	openssl base64 -d -in /tmp/shasum.txt -out /tmp/shasum2.txt
	$PROGRAM base64 -d -i /tmp/shasum.txt -o /tmp/ft_ssl2.txt
	arg=`diff /tmp/ft_ssl2.txt /tmp/shasum2.txt`
	printf "decrypt: "
	if [[ -z  "$arg" ]]; then
		greentext $1
	else
		redtext $1
	fi

}

base64_file_test()
{
	arr=("$@")

	printf "MD5:\n"
	for i in "${arr[@]}"; do
		ABSPATH="$(readlinkf $i)"
		base64_file_reader $ABSPATH
	done
	printf "MD5: END\n\n"
}

base64_file_test "${FILES[@]}"
