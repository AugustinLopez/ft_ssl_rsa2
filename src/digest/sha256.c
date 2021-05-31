/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 11:55:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 23:36:56 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "di_utils.h"

const uint32_t S2XX_CONST[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5
							, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
							, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3
							, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
							, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc
							, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
							, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7
							, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
							, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13
							, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
							, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3
							, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
							, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5
							, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
							, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208
							, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

typedef struct s_sha2xx
{
	size_t		chunk;
	uint32_t	h[8];
	uint32_t	t[8];
}				t_sha2xx;

static t_sha2xx	updatetemp(t_sha2xx ret, uint32_t t1, uint32_t t2)
{
	ret.t[7] = ret.t[6];
	ret.t[6] = ret.t[5];
	ret.t[5] = ret.t[4];
	ret.t[4] = ret.t[3] + t1;
	ret.t[3] = ret.t[2];
	ret.t[2] = ret.t[1];
	ret.t[1] = ret.t[0];
	ret.t[0] = t1 + t2;
	return (ret);
}

static t_sha2xx	updatehash(t_sha2xx a, uint32_t *word)
{
	uint32_t	rotate;
	uint32_t	xoring;
	uint32_t	temp1;
	uint32_t	temp2;

	for (int i = 0; i < 64; i++) {
		rotate = r_32rot(a.t[4], 6) ^ r_32rot(a.t[4], 11) ^ r_32rot(a.t[4], 25);
		xoring = (a.t[4] & a.t[5]) ^ (a.t[6] & ~a.t[4]);
		temp1 = a.t[7] + rotate + xoring + S2XX_CONST[i] + word[i];
		rotate = r_32rot(a.t[0], 2) ^ r_32rot(a.t[0], 13) ^ r_32rot(a.t[0], 22);
		xoring = (a.t[0] & a.t[1]) ^ (a.t[0] & a.t[2]) ^ (a.t[1] & a.t[2]);
		temp2 = rotate + xoring;
		a = updatetemp(a, temp1, temp2);
	}
	for (int i = 0;i < 8; i++)
		a.h[i] += a.t[i];
	for (int i = 0;i < 8; i++)
		a.t[i] = a.h[i];
	return (a);
}

static uint32_t	*fill_word(uint32_t *word, uint8_t *s)
{
	uint32_t	s0;
	uint32_t	s1;

	for (int i = 0; i < 64; i++)
		word[i] = 0;
	uint8to32(word, s, 16, 0);
	for (int i = 16; i < 64; i++) {
		s0 = r_32rot(word[i - 15], 7) ^ r_32rot(word[i - 15], 18)
			^ (word[i - 15] >> 3);
		s1 = r_32rot(word[i - 2], 17) ^ r_32rot(word[i - 2], 19)
			^ (word[i - 2] >> 10);
		word[i] = word[i - 16] + s0 + word[i - 7] + s1;
	}
	return (word);
}

static void		init(t_sha2xx *algo, size_t len, int sha)
{
	algo->chunk = len / 64 + ((len % 64) != 0);
	if (sha == 256) {
		algo->h[0] = 0x6a09e667;
		algo->h[1] = 0xbb67ae85;
		algo->h[2] = 0x3c6ef372;
		algo->h[3] = 0xa54ff53a;
		algo->h[4] = 0x510e527f;
		algo->h[5] = 0x9b05688c;
		algo->h[6] = 0x1f83d9ab;
		algo->h[7] = 0x5be0cd19;
	}
	else if (sha == 224) {
		algo->h[0] = 0xc1059ed8;
		algo->h[1] = 0x367cd507;
		algo->h[2] = 0x3070dd17;
		algo->h[3] = 0xf70e5939;
		algo->h[4] = 0xffc00b31;
		algo->h[5] = 0x68581511;
		algo->h[6] = 0x64f98fa7;
		algo->h[7] = 0xbefa4fa4;
	}
	for (int i = 0;i < 8; i++)
		algo->t[i] = algo->h[i];
}

static t_sha2xx sha2xx_algorithm(uint8_t *s, size_t len, int sha)
{
	t_sha2xx	algo;
	uint32_t	word[64];

	init(&algo, len, sha);
	for (size_t i = 0; i < algo.chunk; i++) {
		fill_word(word, s + i * 64);
		algo = updatehash(algo, word);
	}
	return (algo);
}

static char *sha2xx_ret(t_sha2xx *algo, int sha)
{
	char *ret;
	uint8_t c;
	int len = sha == 256 ? 8 : 7;
	uint32_t *tab = algo->h;

	ret = malloc((8 * len + 1) * sizeof(*ret));
	if (!ret)
		return (NULL);
	ret[8 * len] = '\0';
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < 8; j++) {
			c = (tab[i] >> (28 - 4 * j)) & 0xf;
			if (c < 10)
				ret[i * 8 + j] = '0' + c;
			else
				ret[i * 8 + j] = 'a' + c - 10;
		}
	}
	return (ret);
}

char			*sha256(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_sha2xx	algo;

	str = di_preprocessing(&padded_len, tocrypt, len, 0);
	if (str == NULL)
		return (NULL);
	algo = sha2xx_algorithm((uint8_t *)str, padded_len, 256);
	free(str);
	return (sha2xx_ret(&algo, 256));
}

char			*sha224(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_sha2xx	algo;

	str = di_preprocessing(&padded_len, tocrypt, len, 0);
	if (str == NULL)
		return (NULL);
	algo = sha2xx_algorithm((uint8_t *)str, padded_len, 224);
	free(str);
	return (sha2xx_ret(&algo, 224));
}
