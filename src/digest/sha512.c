/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha512.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 11:55:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/27 15:18:46 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "digest/sha2.h"
#include "common/ssl_string.h"
#include <stdio.h>
#include <string.h>

static void		init(t_sha5xx *algo, size_t len, int sha)
{
	algo->chunk = (len + 17) / 128 + (((len + 17) % 128) != 0);
	if (sha == 512)
	{
		algo->h[0] = 0x6a09e667f3bcc908;
		algo->h[1] = 0xbb67ae8584caa73b;
		algo->h[2] = 0x3c6ef372fe94f82b;
		algo->h[3] = 0xa54ff53a5f1d36f1;
		algo->h[4] = 0x510e527fade682d1;
		algo->h[5] = 0x9b05688c2b3e6c1f;
		algo->h[6] = 0x1f83d9abfb41bd6b;
		algo->h[7] = 0x5be0cd19137e2179;
	}
	else if (sha == 512256 || sha == 512224)
	{
		algo->h[0] = 0x6a09e667f3bcc908 ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[1] = 0xbb67ae8584caa73b ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[2] = 0x3c6ef372fe94f82b ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[3] = 0xa54ff53a5f1d36f1 ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[4] = 0x510e527fade682d1 ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[5] = 0x9b05688c2b3e6c1f ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[6] = 0x1f83d9abfb41bd6b ^ 0xa5a5a5a5a5a5a5a5;
		algo->h[7] = 0x5be0cd19137e2179 ^ 0xa5a5a5a5a5a5a5a5;
	}
	else if (sha == 384) {
		algo->h[0] = 0xcbbb9d5dc1059ed8;
		algo->h[1] = 0x629a292a367cd507;
		algo->h[2] = 0x9159015a3070dd17;
		algo->h[3] = 0x152fecd8f70e5939;
		algo->h[4] = 0x67332667ffc00b31;
		algo->h[5] = 0x8eb44a8768581511;
		algo->h[6] = 0xdb0c2e0d64f98fa7;
		algo->h[7] = 0x47b5481dbefa4fa4;
	}
	memcpy(algo->t, algo->h, 8 * sizeof(uint64_t));
}

char			*sha512(uint8_t *tocrypt, size_t len)
{
	t_string	buffer;
	t_sha5xx	algo;
	char		*re;
	unsigned long long *tmp;

	buffer = dig_lpreprocessing(tocrypt, len, 0);
	if (buffer.err)
		return (NULL);
	init(&algo, len, 512);
	algo = sha5xx_algorithm(buffer.s, algo);
	free_string(&buffer);
	tmp = (unsigned long long *)algo.h;
	asprintf(&re, "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx"
		, tmp[0], tmp[1], tmp[2], tmp[3]
		, tmp[4], tmp[5], tmp[6], tmp[7]);
	return (re);
}

char			*sha384(uint8_t *tocrypt, size_t len)
{
	t_string	buffer;
	t_sha5xx	algo;
	char		*re;
	unsigned long long *tmp;

	buffer = dig_lpreprocessing(tocrypt, len, 0);
	if (buffer.err)
		return (NULL);
	init(&algo, len, 384);
	algo = sha5xx_algorithm(buffer.s, algo);
	free_string(&buffer);
	tmp = (unsigned long long *)algo.h;
	asprintf(&re, "%016llx%016llx%016llx%016llx%016llx%016llx"
		, tmp[0], tmp[1], tmp[2], tmp[3]
		, tmp[4], tmp[5]);
	return (re);
}

char			*sha512_256(uint8_t *tocrypt, size_t len)
{
	t_string	buffer;
	t_string	prebuff;
	t_sha5xx	algo;
	char		*re;
	uint8_t		tmp[11];
	unsigned long long *tmp2;

	memcpy(tmp, "SHA-512/256", 11);
	prebuff = dig_lpreprocessing((uint8_t *)tmp, 11, 0);
	if (prebuff.err)
		return (NULL);
	init(&algo, 11, 512256);
	algo = sha5xx_algorithm(prebuff.s, algo);
	free_string(&prebuff);
	buffer = dig_lpreprocessing(tocrypt, len, 0);
	if (buffer.err)
		return (NULL);
	algo.chunk = (len + 17) / 128 + (((len + 17) % 128) != 0);
	algo = sha5xx_algorithm(buffer.s, algo);
	free_string(&buffer);
	tmp2 = (unsigned long long *)algo.h;
	asprintf(&re, "%016llx%016llx%016llx%016llx"
		, tmp2[0], tmp2[1], tmp2[2], tmp2[3]);
	return (re);
}

char			*sha512_224(uint8_t *tocrypt, size_t len)
{
	t_string	buffer;
	t_string	prebuff;
	t_sha5xx	algo;
	char		*re;
	uint8_t		tmp[11];
	unsigned long long *tmp2;

	memcpy(tmp, "SHA-512/224", 11);
	prebuff = dig_lpreprocessing((uint8_t *)tmp, 11, 0);
	if (prebuff.err)
		return (NULL);
	init(&algo, 11, 512224);
	algo = sha5xx_algorithm(prebuff.s, algo);
	free_string(&prebuff);
	buffer = dig_lpreprocessing(tocrypt, len, 0);
	if (buffer.err)
		return (NULL);
	algo.chunk = (len + 17) / 128 + (((len + 17) % 128) != 0);
	algo = sha5xx_algorithm(buffer.s, algo);
	free_string(&buffer);
	tmp2 = (unsigned long long *)algo.h;
	asprintf(&re, "%016llx%016llx%016llx%08llx"
		, tmp2[0], tmp2[1], tmp2[2], (tmp2[3] >> 32));
	return (re);
}
