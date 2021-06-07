rnd=$(( ( RANDOM % 32 )  + 1 ))
pass="$(openssl rand -hex $rnd)"
salt="$(openssl rand -hex 8)"
iv="$(openssl rand -hex 8)"
openssl $1 -md md5 -pass "pass:$pass" -S $salt -in $2 -iv $iv -out /tmp/their.txt -a
../ft_ssl $1 -p $pass -s $salt -i $2 -o /tmp/mine.txt -v $iv -a
../ft_ssl $1 -d -p $pass -s $salt -i /tmp/their.txt -v $iv -o /tmp/result.txt -a
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
	echo "CMD=$1\nFILE=$2\npass=$pass\nsalt=$salt\n" >> error.txt
fi
