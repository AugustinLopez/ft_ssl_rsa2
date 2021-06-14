/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   genrsa.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/10 13:34:13 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/27 14:52:00 by aulopez          ###   ########.fr       */
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

uint64_t gcd(uint64_t a, uint64_t b)
{
	uint64_t ret;
	uint64_t temp;

	while (b) {
		temp = b;
		b = a % b;
		a = temp;
	}
	return (a);
}

uint64_t lcm(uint64_t a, uint64_t b)
{
	uint64_t div;

	div = gcd(a, b);
	return ((a * b) / div);
}

int64_t modInverse(int a, int m)
{
	int64_t m0;
	int64_t y;
	int64_t x;
	int64_t q;
	int64_t t;

	m0 = m;
	y = 0;
	x = 1;
	if (m == 1)
		return 0;

	while (a > 1) {
		// q is quotient
		int q = a / m;
		int t = m;

		// m is remainder now, process same as
		// Euclid's algo
		m = a % m, a = t;
		t = y;

		// Update y and x
		y = x - q * y;
		x = t;
	}
	// Make x positive
	if (x < 0)
	x += m0;

	return x;
}
int64_t mul_inv(uint32_t a, uint32_t b)
{
	int64_t t;
	int64_t nt;
	int64_t r;
	int64_t nr;
	int64_t q;
	int64_t tmp;

	t = 0;
	nt = 1;
	r = b;
	nr = a % b;
	while (nr) {
		q = r/nr;
		tmp = nt;
		nt = t - q*nt;
		t = tmp;
		tmp = nr;
		nr = r - q*nr;
		r = tmp;
	}
	if (r > 1)
		return 0;
	if (t < 0)
		t += b;
	return t;
}

int main(void)
{
	uint64_t p1;
	uint64_t p2;
	uint64_t carmi;
	uint64_t e;
	uint64_t test;

	e = 65537;
//	p1 = 61;
//	p2 = 53;
	while (p1 <= 1)
		p1 = find_prime32();
	while (p2 <= 1)
		p2 = find_prime32();
	carmi = lcm(p1 - 1, p2 - 1);
	test = gcd(e, carmi);
//	printf("%llu\n", modInverse(17, 780));
	printf("%llu %llu %llu %llu %llu\n", p1, p2, p1 * p2, carmi, test);
	return (0);
}


