/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prime_generator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/10 13:34:13 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/11 10:47:37 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

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

	//subject expect a probabilistic version. But for 64-bit, the deterministic
	//is fast enough.
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

uint64_t		ft_rand2(void)
{
	int			fd;
	ssize_t		len;
	uint64_t	ret;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (0);
	len = read(fd, &ret, sizeof(ret));
	return (ret);
}

uint64_t find_prime64(void) {
	uint64_t	ret;
	int			test;

	ret = ft_rand2();
	test = miller_rabbin(ret, 100);
	while (1) {
		if (test == 1)
			break ;
		test = miller_rabbin(--ret, 100);
	}
	return (ret);
}

uint32_t find_prime32(void) {
	uint64_t	ret;
	int			test;

	ret = (uint32_t)ft_rand2();
	test = miller_rabbin(ret, 100);
	while (1) {
		if (test == 1)
			break ;
		test = miller_rabbin(--ret, 100);
	}
	return (ret);
}


int main(void)
{
	uint64_t prime;

	prime = find_prime32();
	printf("%llu\n", prime);
	return (0);
}
