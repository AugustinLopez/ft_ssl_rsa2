/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prime32.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 10:27:41 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/30 15:57:43 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include "mylib/libft.h"

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

	while (a != 0) {
		if (a % 2 == 1) {
			if (b >= m - res)
				res -= m;
			res += b;
		}
		a >>= 1;

		temp_b = b;
		if (b >= m - b)
			temp_b -= m;
		b += temp_b;
	}
	return res;
}

//Complicated stuff to avoid overflow issue
uint64_t power_mod(uint64_t a, uint64_t n, uint64_t mod)
{
	uint64_t power;
	uint64_t ret;

	power = a;
	ret = 1;
	while (n) {
		if (n % 2 == 1)
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
	if (y != 1)
		return (0);
	return (1);
}

int deterministic_miller_rabbin(uint64_t n)
{
	uint64_t	d;
	uint64_t	s;

	//case 0, 1, 2, 3, mod 2, mod 3
	if (n == 0 || n == 1)
		return (0);
	if (n == 2 || n == 3)
		return (1);
	if (n % 2 == 0 || n % 3 == 0)
		return (0);

	//n can be decomposed as: n = ((2 ^ s) * d) + 1
	d = n / 2;
	s = 1;
	while (d % 2 == 0) {
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


static int subject_with_oversight(uint64_t candidate, uint8_t probability)
{
	int ret;

	//"A probability between 0 and 100 that the given [64-bits] number is a prime.". Oof.

	//1. For 64-bit unsigned integer there is a deterministic algorithm: no need for probability.
	//2. We are not checking if a number is a prime, but if it is NOT a prime (easier). thus,
	//3. There is NEVER a false positive. (case when result says not a prime when prime).
	//   We are optimizing the false negative. (case when result says prime when not a prime)
	//4. The probability depend on a number of round: this should be the function argument instead of the probability, Moreover,
	//5. For any given number and assuming a round number, the probability is at best in a ]0-100[% interval. Narrower in reality.

	ret = deterministic_miller_rabbin(candidate);
	//I will only defend against the case probability = 0%...
	if (probability == 0)
		ret = 1 - ret;
	return (ret);
}

uint32_t find_prime32(uint32_t seed, int use_seed, int output) {
	uint32_t ret;
	int test;

	if (use_seed)
		ret = seed;
	else
		ret = (uint32_t)ft_rand();
	ret |= 0xc0000000; //first bit active to have a 32 bit size.
					//and we need something greater than or equal to 0x8000000b
					//so we activate first 2 bit and call it a day.

	test = subject_with_oversight(ret, 100);
	if (output)
		write(STDERR_FILENO, ".", 1);
	while (1) {
		if (test == 1)
			break ;
		test = subject_with_oversight(++ret, 100);
		if (output)
			write(STDERR_FILENO, ".", 1);
	}
	if (output)
		write(STDERR_FILENO, "*\n", 2);
	return (ret);
}
