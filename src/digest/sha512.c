/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha512.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 11:55:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/01 00:54:03 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "di_utils.h"

typedef struct s_sha5xx
{
	size_t		chunk;
	uint64_t	h[8];
	uint64_t	t[8];
}				t_sha5xx;

const uint64_t	S5XX_CONST[80] =
	{0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f
	, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019
	, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242
	, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2
	, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235
	, 0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3
	, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275
	, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5
	, 0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f
	, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725
	, 0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc
	, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df
	, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6
	, 0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001
	, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218
	, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8
	, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99
	, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb
	, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc
	, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec
	, 0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915
	, 0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207
	, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba
	, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b
	, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc
	, 0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a
	, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

static t_sha5xx	updatetemp(t_sha5xx ret, uint64_t t1, uint64_t t2)
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

static t_sha5xx	updatehash(t_sha5xx a, uint64_t *word)
{
	uint64_t	urot;
	uint64_t	uxor;
	uint64_t	temp1;
	uint64_t	temp2;

	for (int i = 0; i < 80; i++) {
		urot = r_64rot(a.t[4], 14) ^ r_64rot(a.t[4], 18) ^ r_64rot(a.t[4], 41);
		uxor = (a.t[4] & a.t[5]) ^ (a.t[6] & ~a.t[4]);
		temp1 = a.t[7] + urot + uxor + S5XX_CONST[i] + word[i];
		urot = r_64rot(a.t[0], 28) ^ r_64rot(a.t[0], 34) ^ r_64rot(a.t[0], 39);
		uxor = (a.t[0] & a.t[1]) ^ (a.t[0] & a.t[2]) ^ (a.t[1] & a.t[2]);
		temp2 = urot + uxor;
		a = updatetemp(a, temp1, temp2);
	}
	for (int i = 0; i < 8; i++)
		a.h[i] += a.t[i];
	for (int i = 0; i < 8; i++)
		a.t[i] = a.h[i];
	return (a);
}

static uint64_t	*fill_word(uint64_t *word, char *s)
{
	uint64_t	s0;
	uint64_t	s1;

	for (int i = 0; i < 80; i++)
		word[i] = 0;
	uint8to64(word, (uint8_t *)s, 16, 0);
	for (int i = 16; i < 80 ; i++) {
		s0 = r_64rot(word[i - 15], 1) ^ r_64rot(word[i - 15], 8)
			^ (word[i - 15] >> 7);
		s1 = r_64rot(word[i - 2], 19) ^ r_64rot(word[i - 2], 61)
			^ (word[i - 2] >> 6);
		word[i] = word[i - 16] + s0 + word[i - 7] + s1;
	}
	return (word);
}

static t_sha5xx		sha5xx_algorithm(char *s, t_sha5xx algo)
{
	uint64_t	word[80];

	for (size_t i = 0; i < algo.chunk; i++) {
		fill_word(word, s + i * 128);
		algo = updatehash(algo, word);
	}
	return (algo);
}

static void		init(t_sha5xx *algo, size_t len, int sha)
{
	algo->chunk = len / 128 + ((len % 128) != 0);
	if (sha == 512) {
		algo->h[0] = 0x6a09e667f3bcc908;
		algo->h[1] = 0xbb67ae8584caa73b;
		algo->h[2] = 0x3c6ef372fe94f82b;
		algo->h[3] = 0xa54ff53a5f1d36f1;
		algo->h[4] = 0x510e527fade682d1;
		algo->h[5] = 0x9b05688c2b3e6c1f;
		algo->h[6] = 0x1f83d9abfb41bd6b;
		algo->h[7] = 0x5be0cd19137e2179;
	}
	else if (sha == 512256 || sha == 512224) {
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
	for (int i = 0; i < 8; i++)
		algo->t[i] = algo->h[i];
}

static char *sha5xx_ret(t_sha5xx *algo, int sha)
{
	char *ret;
	uint8_t c;
	int len;
	uint64_t *tab = algo->h;

	if (sha == 512)
		len = 8;
	else if (sha == 384)
		len = 6;
	else
		len = 4;
	ret = malloc((16 * len + 1) * sizeof(*ret));
	if (!ret)
		return (NULL);
	ret[16 * len] = '\0';
	for (int i = 0; i < len; i++) {
		for (int j = 0; j < 16; j++) {
			c = (tab[i] >> (60 - 4 * j)) & 0xf;
			if (c < 10)
				ret[i * 16 + j] = '0' + c;
			else
				ret[i * 16 + j] = 'a' + c - 10;
		}
	}
	if (sha == 512224)
		ret[16 * len - 8] = '\0';
	return (ret);
}

char *sha512(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_sha5xx	algo;

	str = di_lpreprocessing(&padded_len, tocrypt, len, 0);
	if (str == NULL)
		return (NULL);
	init(&algo, padded_len, 512);
	algo = sha5xx_algorithm(str, algo);
	free(str);
	return (sha5xx_ret(&algo, 512));
}

char *sha384(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_sha5xx	algo;

	str = di_lpreprocessing(&padded_len, tocrypt, len, 0);
	if (str == NULL)
		return (NULL);
	init(&algo, padded_len, 384);
	algo = sha5xx_algorithm(str, algo);
	free(str);
	return (sha5xx_ret(&algo, 384));
}

char *sha512_256(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_sha5xx	algo;
	char		tmp[11] = "SHA-512/256";

	str = di_lpreprocessing(&padded_len, (char *)tmp, 11, 0);
	if (str == NULL)
		return (NULL);
	init(&algo, padded_len, 512256);
	algo = sha5xx_algorithm(str, algo);
	free(str);
	str = di_lpreprocessing(&padded_len, tocrypt, len, 0);
	if (str == NULL)
		return (NULL);
	algo.chunk = (len + 17) / 128 + (((len + 17) % 128) != 0);
	algo = sha5xx_algorithm(str, algo);
	free(str);
	return(sha5xx_ret(&algo, 512256));
}

char *sha512_224(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_sha5xx	algo;
	char		tmp[11] = "SHA-512/224";

	str = di_lpreprocessing(&padded_len, (char *)tmp, 11, 0);
	if (str == NULL)
		return (NULL);
	init(&algo, padded_len, 512224);
	algo = sha5xx_algorithm(str, algo);
	free(str);
	str = di_lpreprocessing(&padded_len, tocrypt, len, 0);
	if (str == NULL)
		return (NULL);
	algo.chunk = (len + 17) / 128 + (((len + 17) % 128) != 0);
	algo = sha5xx_algorithm(str, algo);
	free(str);
	return(sha5xx_ret(&algo, 512224));
}
