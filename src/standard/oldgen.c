/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oldgen.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/10 13:34:13 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/14 16:42:18 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

int64_t mul_inv(uint64_t a, uint64_t b)
{
	int64_t t;
	int64_t nt;
	uint64_t r;
	uint64_t nr;
	uint64_t q;
	uint64_t tmp;

	t = 0;
	nt = 1;
	r = b;
	nr = a % b;
	while (nr) {
		q = r/nr;
		tmp = nt;
		nt = t - (int64_t)q*nt;
		t = tmp;
		tmp = nr;
		nr = r - q*nr;
		r = tmp;
	}
	if (r > 1)
		return 0;
	if (t < 0)
		t += b;
	printf("%llu\n", (uint64_t)t);
	return t;
}

uint64_t mul_inv2(uint64_t a, uint64_t b)
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


uint64_t extended_euclidean(uint64_t integer, uint64_t modulo)
{
	int64_t a[3] = {0, 17, 780};
	int64_t x[3] = {1, 0, 0};
	int64_t y[3] = {0, 1, 0};
	int64_t q = 0;
	int64_t qq = 0;

	while (a[2]) {
		qq = (int64_t)q;
		x[2] = x[0] - qq * x[1];
		y[2] = y[0] - qq * y[1];
		x[0] = x[1];
		y[0] = y[1];
		a[0] = a[1];
		x[1] = x[2];
		y[1] = y[2];
		a[1] = a[2];
		q = a[0] / a[1];
		a[2] = a[0] - q * a[1];
	}
	printf("%lli - %llx\n",  y[1], y[1]);
	return (x[1]);
}

/*
void extended_euclidean(const U a, const U b, U* pGcd, S* pX, S* pY)
{
   static_assert(std::numeric_limits<S>::is_integer, "");
   static_assert(std::numeric_limits<S>::is_signed, "");
   static_assert(std::numeric_limits<U>::is_integer, "");
   static_assert(!(std::numeric_limits<U>::is_signed), "");
   static_assert(std::is_same<std::make_signed<U>::type, S>::value, "");
   S x1=1, y1=0;
   U a1=a;
   S x0=0, y0=1;
   U a2=b, q=0;

   while (a2 != 0) {
      S x2 = x0 - static_cast<S>(q)*x1;
      S y2 = y0 - static_cast<S>(q)*y1;
      x0=x1; y0=y1;
      U a0=a1;
      x1=x2; y1=y2; a1=a2;

      q = a0/a1;
      a2 = a0 - q*a1;
   }
   *pX = x1;
   *pY = y1;
   *pGcd = a1;
}*/

uint64_t modmulinv(uint64_t integer, uint64_t modulo) {
	uint64_t r[2] = {17, 780};
	int64_t s[2] = {1, 0};
	int64_t t[2] = {0, 1};

	while (r[1]) {
		uint64_t quotient = r[0] / r[1];
		uint64_t tmp = r[1];
		r[1] = r[0] - quotient * tmp;
		r[0] = tmp;
		tmp = s[1];
		s[1] = s[0] - (int64_t)quotient * tmp;
		s[0] = tmp;
		tmp = t[1];
		t[1] = t[0] - (int64_t)quotient * tmp;
		t[0] = tmp;
	}
	printf("%lli %lli\n", r[0], r[1]);
	return (s[0]);
/*function extended_gcd(a, b)
(old_r, r) := (r, old_r - quotient *r) //
//
prov := r;
r := old_r - quotient × prov;
old_r := prov;
//
    (old_r, r) := (a, b)
    (old_s, s) := (1, 0)
    (old_t, t) := (0, 1)
    
    while r ≠ 0 do
        quotient := old_r div r
        (old_r, r) := (r, old_r − quotient × r)
        (old_s, s) := (s, old_s − quotient × s)
        (old_t, t) := (t, old_t − quotient × t)*/
}





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


uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod)
{
    uint64_t res = 0; // Initialize result
    a = a % mod;
    while (b > 0)
    {
        // If b is odd, add 'a' to result
        if (b % 2 == 1)
            res = (res + a) % mod;
 
        // Multiply 'a' with 2
        a = (a * 2) % mod;
 
        // Divide b by 2
        b /= 2;
    }
 
    // Return result
    return res % mod;
}

int main(void)
{
	uint64_t p1;
	uint64_t p2;
	uint64_t carmi;
	uint64_t e;
	uint64_t test;

	e = 65537;
	p1 = 0xf8b6215b;
	p2 = 0xf15b58c5;
	while (p1 <= 1)
		p1 = find_prime32();
	while (p2 <= 1)
		p2 = find_prime32();
	mul_inv(e, 16896437491524335848ULL);
	mul_inv2(e, 16896437491524335848ULL);
//	carmi = lcm(p1 - 1, p2 - 1);
	/*carmi = (p1 - 1) * ( p2 - 1);
	printf("p   : %llu - %llx\n", p1, p1);
	printf("q   : %llu - %llx\n", p2, p2);
	printf("n   : %llu - %llx\n", p1 * p2, p1 * p2);
	printf("test: %llu - %llx\n", modmulinv(e, 16896437491524335848ULL), extended_euclidean(e, 16896437491524335848ULL));
	printf("lcm : %llu - %llx\n", (p1 - 1) * (p2 - 1), (p1 - 1) * (p2 - 1));
	printf("test: %llu - %llx\n", 9221019932585553137ULL, 9221019932585553137ULL);
	test = gcd(e, carmi);
	printf("%llu\n", carmi);
	printf("%llx\n", 772801186823385213LL);
	printf("%llx\n", modInverse(e, carmi));
	printf("%llx %llx %llx %llx %llx\n", p1, p2, p1 * p2, carmi, test);
	*/return (0);
}
