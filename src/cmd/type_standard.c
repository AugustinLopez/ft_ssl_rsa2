/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_standard.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 01:28:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/14 17:16:06 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"
#include <unistd.h>
#include <limits.h>

//Very complicated stuff to avoid overflow issue
//Stackoverflow 12168348. Some kind of Russian Peasant multiplication
//Thanks you Craig McQueen
static uint64_t mult_mod(uint64_t a, uint64_t b, uint64_t m) {
	uint64_t res = 0;
	uint64_t temp_b;

	if (b >= m) {
		if (m > ULLONG_MAX / 2u)
			b -= m;
		else
			b %= m;
	}

	while ( a != 0) {
		if (a & 1) {
			if (b >= m - res)
				res -= m;
			res += b;
		}
		a >>= 1;

		temp_b = b;
		if (b >= m - b)
			temp_b -= m;
		b+= temp_b;
	}
	return res;
}

//Complicated stuff to avoid overflow issue
static uint64_t power_mod(uint64_t a, uint64_t n, uint64_t mod)
{
	uint64_t power;
	uint64_t ret;

	power = a;
	ret = 1;
	while (n) {
		if (n & 1)
			ret = mult_mod(ret, power, mod);
		power = mult_mod(power, power, mod);
		n >>= 1;
	}
	return (ret);
}

//Check Wikipedia: Miller-Rabin primality Test#Variant for finding factors
static int witness(uint64_t n, uint64_t s, uint64_t d, uint64_t a)
{
	uint64_t x;
	uint64_t y;

	x = power_mod(a, d, n);
	while (s) {
		y = mult_mod(x, x, n);
		if (y == 1 && x != 1 && x != n - 1) {
			return (0);
		}
		x = y;
		--s;
	}
	//equivalent to x != 1
	if (y != 1)
		return (0);
	return (1);
}

static int miller_rabbin(uint64_t n, uint8_t probability)
{
	uint64_t	d;
	uint64_t	s;

	//subject expect a probabilistic version. But for 64-bit, we can do deterministic.
	(void)probability;

	//case 0, 1, 2, 3, mod 2, mod 3
	//This is because n must be > 3 in this implementation of the witness loop
	if (((!(n & 1)) && n != 2) || (n < 2) || (n % 3 == 0 && n != 3))
		return (0);
	if (n <= 3)
		return (1);

	//n = ((2 ^ s) * d) + 1
	d = n / 2;
	s = 1;
	while (!(d & 1)) {
		d /= 2;
		++s;
	}

	//Known simplifications of the Miller test for 64-bit.
	if (n < 1373653)
		return witness(n, s, d, 2) && witness(n, s, d, 3);
	if (n < 9080191)
		return witness(n, s, d, 31) && witness(n, s, d, 73);
	if (n < 4759123141)
		return witness(n, s, d, 2) && witness(n, s, d, 7) && witness(n, s, d, 61);
	if (n < 1122004669633)
		return witness(n, s, d, 2) && witness(n, s, d, 13)
		&& witness(n, s, d, 23) && witness(n, s, d, 1662803);
	if (n < 2152302898747)
		return witness(n, s, d, 2) && witness(n, s, d, 3)
		&& witness(n, s, d, 5) && witness(n, s, d, 7)
		&& witness(n, s, d, 11);
	if (n < 3474749660383)
		return witness(n, s, d, 2) && witness(n, s, d, 3)
		&& witness(n, s, d, 5) && witness(n, s, d, 7)
		&& witness(n, s, d, 11) && witness(n, s, d, 13);
	if (n < 341550071728321)
		return witness(n, s, d, 2) && witness(n, s, d, 3)
		&& witness(n, s, d, 5) && witness(n, s, d, 7)
		&& witness(n, s, d, 11) && witness(n, s, d, 13)
		&& witness(n, s, d, 17);
	if (n < 3825123056546413051)
		return witness(n, s, d, 2) && witness(n, s, d, 3)
		&& witness(n, s, d, 5) && witness(n, s, d, 7)
		&& witness(n, s, d, 11) && witness(n, s, d, 13)
		&& witness(n, s, d, 17) && witness(n, s, d, 19)
		&& witness(n, s, d, 23);
	return witness(n, s, d, 2) && witness(n, s, d, 3)
	&& witness(n, s, d, 5) && witness(n, s, d, 7)
	&& witness(n, s, d, 11) && witness(n, s, d, 13)
	&& witness(n, s, d, 17) && witness(n, s, d, 19)
	&& witness(n, s, d, 23) && witness(n, s, d, 29)
	&& witness(n, s, d, 31) && witness(n, s, d, 37);
}

uint64_t modmulinv(uint64_t a, uint64_t b)
{
	int64_t t[3] = {0, 1, 0};
	uint64_t r[3] = {b, a % b, 0};
	uint64_t q;

	while (r[1]) {
		q = r[0] / r[1];
		t[2] = t[1];
		t[1] = t[0] - (int64_t)q * t[1];
		t[0] = t[2];
		r[2] = r[1];
		r[1] = r[0] - q * r[1];
		r[0] = r[2];
	}
	if (r[0] > 1)
		return (0);
	if (t[0] < 0)
		t[0] += b;
	return ((uint64_t)t[0]);
}

uint32_t find_prime32(void) {
	uint32_t ret;
	int test;

	ret = (uint32_t)ft_rand();
	ret |= 0xc0000000; //first bit active to have a 32 bit size.
					//and we need something greater than or equal to 0x8000000b
					//so we activate first 2 bit and call it a day.

	test = miller_rabbin(ret, 100);
	while (1) {
		if (test == 1)
			break ;
		test = miller_rabbin(++ret, 100);
	}
	return (ret);
}

int addnumber(t_string *str, uint64_t n)
{
	char len;
	char content;

	if (satrail(str, "\x02", 1) == -1)
		return (-1);
	len = 0;
	for (int i = 0; i < 64; i += 8) {
		if ((n >> i) != 0)
			len++;
		else
			break ;
	}
	if (len == 0)
		len = 1;
	content = (n >> ((len - 1) * 8)) & 0xff;
	if ((content & 0x80)) {
		content = len + 1;
		if (satrail(str, &content, 1) == -1 || satrail(str, "\x00", 1) == -1)
			return (-1);
	}
	else if (satrail(str, &len, 1) == -1)
		return (-1);
	for (int i = 64 - len * 8; i < 64; i += 8) {
		content = (n >> (56 - i)) & 0xff;
		if (satrail(str, &content, 1) == -1)
			return (-1);
	}
	return (0);
}

int testout(uint32_t p, uint64_t q, uint64_t e)
{
	t_string *str;
	t_string *out;
	uint64_t d;
	int ret;

	if (salloc(&str, "\x30\x00\x02\x01\x00", 5) == -1)
		return (-1);
	
	//public modulus n
	d = modmulinv(e, (p - 1) * (q - 1));
	ret = 0;
	ret = ret != 0 ? -1 : addnumber(str, p * q);
	ret = ret != 0 ? -1 : addnumber(str, e);
	ret = ret != 0 ? -1 : addnumber(str, d);
	ret = ret != 0 ? -1 : addnumber(str, p);
	ret = ret != 0 ? -1 : addnumber(str, q);
	ret = ret != 0 ? -1 : addnumber(str, d % (p - 1));
	ret = ret != 0 ? -1 : addnumber(str, d % (q - 1));
	ret = ret != 0 ? -1 : addnumber(str, modmulinv(q, p));
	if (ret != 0) {
		//error msg
		return (-1);
	}
	sptr(str)[1] = slen(str) - 2;
	ret = encode_base64(&out, sptr(str), slen(str));
	sfree(str);
	if (ret == -1) {
		//error msg handled by base64?
		return (-1);
	}
	write(STDOUT_FILENO, "-----BEGIN RSA PRIVATE KEY-----\n", 32);
	while (slen(out)) {
		if (slen(out) > 64)
			write(STDOUT_FILENO, sptr(out), 64);
		else
			write(STDOUT_FILENO, sptr(out), slen(out));
		srlead(out, 64);
		write(STDOUT_FILENO, "\n", 1);
	}
	sfree(out);
	write(STDOUT_FILENO, "-----END RSA PRIVATE KEY-----\n", 30);
	return (0);
}

int genrsa_fun(int ac, char **av, t_funinfo *info)
{
	char *fin = NULL;
	char *fout = NULL;

	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "genrsa", av[0], TYPE_STANDARD));
	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDOUT_FILENO, GENRSA_HELP_MSG, ft_strlen(GENRSA_HELP_MSG));
			return (0);
		}
		else if (i == ac - 1) {
			print_err("genrsa", av[i], "invalid use of option", 0);
			return (-1);
		}
		else if (ft_strcmp(av[i], "-i") == 0)
			fin = av[++i];
		else if (ft_strcmp(av[i], "-o") == 0)
			fout = av[++i];
		else {
			print_err("genrsa", av[i], "invalid option", 0);
			return (-1);
		}
	}
	
	testout(find_prime32(), find_prime32(), 0x010001);
	return (0);
}
