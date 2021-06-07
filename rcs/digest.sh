#!/bin/bash

readlinkf(){ perl -MCwd -e 'print Cwd::abs_path shift' "$1";}

FILES=(
	"../Makefile"
	"../inc/ft_digest.h"
	"../ft_ssl"
	"test/zero.txt"
	"test/empty.txt"
	"test/lazydog.txt"
	"`which vim`"
	)

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


shaX_file_reader()
{
	(shasum -a $2 $1 | cut -d ' ' -f 1) > /tmp/shasum.txt
	$PROGRAM sha$2 -q $1 > /tmp/ft_ssl.txt
	arg=`diff /tmp/ft_ssl.txt /tmp/shasum.txt`
	if [[ -z  "$arg" ]]; then
		greentext $1
	else
		redtext $1
	fi
}

shaXT_file_reader()
{
	(shasum -a $2$3 $1 | cut -d ' ' -f 1) > /tmp/shasum.txt
	$PROGRAM sha$2/$3 -q $1 > /tmp/ft_ssl.txt
	arg=`diff /tmp/ft_ssl.txt /tmp/shasum.txt`
	if [[ -z  "$arg" ]]; then
		greentext $1
	else
		redtext $1
	fi
}

shaX_file_test()
{
	arr=("$@")
	arr=("${arr[@]:1}")

	printf "SHA$1:\n"
	for i in "${arr[@]}"; do
		ABSPATH="$(readlinkf $i)"
		shaX_file_reader $ABSPATH $1
	done
	printf "SHA$1: END\n\n"
}

shaXT_file_test()
{
	arr=("$@")
	arr=("${arr[@]:2}")

	printf "SHA$1/$2:\n"
	for i in "${arr[@]}"; do
		ABSPATH="$(readlinkf $i)"
		shaXT_file_reader $ABSPATH $1 $2
	done
	printf "SHA$1/$2: END\n\n"
}

md5_file_reader()
{
	(md5 $1 | cut -d ' ' -f 4) > /tmp/shasum.txt
	$PROGRAM md5 -q $1 > /tmp/ft_ssl.txt
	arg=`diff /tmp/ft_ssl.txt /tmp/shasum.txt`
	if [[ -z  "$arg" ]]; then
		greentext $1
	else
		redtext $1
	fi
}

md5_file_test()
{
	arr=("$@")

	printf "MD5:\n"
	for i in "${arr[@]}"; do
		ABSPATH="$(readlinkf $i)"
		md5_file_reader $ABSPATH
	done
	printf "MD5: END\n\n"
}

subject()
{
	echo "pickle rick" | $PROGRAM md5 > /tmp/correction.txt
	echo "Do not pity the dead, Harry." | $PROGRAM md5 -p >> /tmp/correction.txt
	echo "Pity the living." | $PROGRAM md5 -q -r >> /tmp/correction.txt
	echo "And above all," > /tmp/file
	$PROGRAM md5 /tmp/file >> /tmp/correction.txt
	$PROGRAM md5 -r /tmp/file >> /tmp/correction.txt
	$PROGRAM md5 -s "pity those that aren't following baerista on spotify." >> /tmp/correction.txt
	echo "be sure to handle edge cases carefully" | $PROGRAM md5 -p /tmp/file >> /tmp/correction.txt
	echo "some of this will not make sense at first" | $PROGRAM md5 /tmp/file >> /tmp/correction.txt
	echo "but eventually you will understand" | $PROGRAM md5 -p -r /tmp/file >> /tmp/correction.txt
	echo "GL HF let's go" | $PROGRAM md5 -p -s "foo" /tmp/file >> /tmp/correction.txt
	echo "one more thing" | $PROGRAM md5 -r -p -s "foo" /tmp/file -s "bar" >> /tmp/correction.txt
	echo "just to be extra clear" | $PROGRAM md5 -r -q -p -s "foo" /tmp/file >> /tmp/correction.txt
	echo "https://www.youtube.com/watch?v=w-5yAtMtrSM" > /tmp/big_smoke_order_remix
	$PROGRAM sha256 -q /tmp/big_smoke_order_remix >> /tmp/correction.txt
	$PROGRAM sha256 -s "wubba lubba dub dub" >> /tmp/correction.txt
	cat /tmp/correction.txt
}

subject
md5_file_test "${FILES[@]}"
shaX_file_test 256 "${FILES[@]}"
shaX_file_test 224 "${FILES[@]}"
shaX_file_test 512 "${FILES[@]}"
shaX_file_test 384 "${FILES[@]}"
shaXT_file_test 512 224 "${FILES[@]}"
shaXT_file_test 512 256 "${FILES[@]}"
