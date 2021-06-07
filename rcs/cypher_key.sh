key="$(openssl rand -hex 24)"
iv="$(openssl rand -hex 8)"
openssl $1 -K $key -iv $iv -in $2 -out /tmp/their.txt -a
../ft_ssl $1 -k $key -v $iv -i $2 -o /tmp/mine.txt -a
../ft_ssl $1 -d -k $key -v $iv -i /tmp/their.txt  -o /tmp/result.txt -a
diff1="$(diff /tmp/mine.txt /tmp/their.txt)"
diff2="$(diff /tmp/result.txt $2)"
i=0
if [ -z "$diff1" ]; then
	printf "\e[32mEncode OK\e[0m\n"
else
	printf "\e[31mEncode KO\e[0m\n"
	i=1
fi
if [ -z "$diff2" ]; then
	printf "\e[32mDecode OK\e[0m\n"
else
	printf "\e[31mDecode KO\e[0m\n"
	i=1
fi
if [ $i -ne 0 ]; then
	echo "CMD=$1\nFILE=$2\nkey=$key\niv =$iv\n" >> error.txt
fi
