/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   md5.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/27 19:09:44 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 22:18:23 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "mylib/libft.h"

typedef struct s_md5
{
	size_t		chunk;
	uint32_t	a0;
	uint32_t	b0;
	uint32_t	c0;
	uint32_t	d0;
	uint32_t	a;
	uint32_t	b;
	uint32_t	c;
	uint32_t	d;
	uint32_t	g;
	uint32_t	f;
}				t_md5;

const uint32_t MD5_SHIFT[64] = {7, 12, 17, 22, 7, 12, 17, 22
							, 7, 12, 17, 22, 7, 12, 17, 22
							, 5, 9, 14, 20, 5, 9, 14, 20
							, 5, 9, 14, 20, 5, 9, 14, 20
							, 4, 11, 16, 23, 4, 11, 16, 23
							, 4, 11, 16, 23, 4, 11, 16, 23
							, 6, 10, 15, 21, 6, 10, 15, 21
							, 6, 10, 15, 21, 6, 10, 15, 21};
const uint32_t MD5_CONST[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee
							, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501
							, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be
							, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
							, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa
							, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8
							, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed
							, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
							, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c
							, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70
							, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05
							, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
							, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039
							, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1
							, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1
							, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};


static inline void swap_byte(uint8_t *a, uint8_t *b)
{
	if (*a != *b) {
		*a = *a ^ *b;
		*b = *b ^ *a;
		*a = *a ^ *b;
	}
}

static inline uint32_t l_32rot(uint32_t x, size_t shift)
{
	return ((x << shift) | (x >> (32 - shift)));
}

static void	*uint8to32(uint32_t *dest, uint8_t *src, size_t len, int little_endian)
{
	if (little_endian == 1) {
		for (size_t i = 0; i < len; i++)
			dest[i] = src[i * 4] + (src[i * 4 + 1] << 8)
			+ (src[i * 4 + 2] << 16) + (src[i * 4 + 3] << 24);
	}
	else {
		for (size_t i = 0; i < len; i++)
			dest[i] = (src[i * 4] << 24) + (src[i * 4 + 1] << 16)
			+ (src[i * 4 + 2] << 8) + src[i * 4 + 3];
	}
	return (dest);
}

static char *di_preprocessing(size_t *padded_len, char *tocrypt, size_t len, int little_endian)
{
	char	tmp[72];
	char	*ret;
	int	pad;
	int	i;

	ft_memset(tmp, 0, 72);
	tmp[0] = 0x80;
	pad = 55 - len % 64;
	if (pad < 0)
		pad += 64;
	i = -1;
	if (little_endian == 1)
		while (++i < 8)
			tmp[pad + 8 - i] = ((len * 8) >> (56 - i * 8)) & 0xff;
	else
		while (++i < 8)
			tmp[pad + 1 + i] = ((len * 8) >> (56 - i * 8)) & 0xff;
	ret = malloc(len + pad + 9);
	if (!ret)
		return (NULL);
	ft_memcpy(ret, tocrypt, len);
	ft_memcpy(ret + len, tmp, pad + 9);
	*padded_len = len + pad + 9;
	return (ret);
}

static t_md5	update(t_md5 algo)
{
	algo.a0 += algo.a;
	algo.b0 += algo.b;
	algo.c0 += algo.c;
	algo.d0 += algo.d;
	algo.a = algo.a0;
	algo.b = algo.b0;
	algo.c = algo.c0;
	algo.d = algo.d0;
	return (algo);
}

static t_md5	init(size_t len)
{
	t_md5	ret;

	ret.chunk = len / 64 + ((len % 64) != 0);
	ret.a0 = 0x67452301;
	ret.b0 = 0xefcdab89;
	ret.c0 = 0x98badcfe;
	ret.d0 = 0x10325476;
	ret.a = ret.a0;
	ret.b = ret.b0;
	ret.c = ret.c0;
	ret.d = ret.d0;
	return (ret);
}

static uint32_t	update_val(char c, t_md5 algo, int i)
{
	if (c == 'f') {
		if (i < 16)
			return ((algo.b & algo.c) | (algo.d & ~algo.b));
		else if (i < 32)
			return ((algo.d & algo.b) | (algo.c & ~algo.d));
		else if (i < 48)
			return (algo.b ^ algo.c ^ algo.d);
		else
			return (algo.c ^ (algo.b | ~algo.d));
	}
	if (i < 16)
		return (i);
	else if (i < 32)
		return ((5 * i + 1) % 16);
	else if (i < 48)
		return ((3 * i + 5) % 16);
	else
		return ((7 * i) % 16);
}

static t_md5	algorithm(uint8_t *s, size_t len)
{
	t_md5		algo;
	uint32_t	temp;
	size_t		j;
	int			i;

	algo = init(len);
	j = 0;
	while (j < algo.chunk)
	{
		i = -1;
		while (++i < 64)
		{
			algo.g = update_val('g', algo, i);
			uint8to32(&temp, s + j * 64 + algo.g * 4, 1, 1);
			algo.f = update_val('f', algo, i) + algo.a + MD5_CONST[i] + temp;
			algo.a = algo.d;
			algo.d = algo.c;
			algo.c = algo.b;
			algo.b += l_32rot(algo.f, MD5_SHIFT[i]);
		}
		algo = update(algo);
		++j;
	}
	return (algo);
}

static char *md5_ret(t_md5 *algo)
{
	char *ret;
	uint8_t c;
	uint32_t tab[4] = {algo->a0, algo->b0, algo->c0, algo->d0};

	ret = malloc(33 * sizeof(*ret));
	if (!ret)
		return (NULL);
	ret[32] = '\0';
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 8; j++) {
			c = (tab[i] >> (28 - 4 * j)) & 0xf;
			if (c < 10)
				ret[i * 8 + j] = '0' + c;
			else
				ret[i * 8 + j] = 'a' + c - 10;
		}
		swap_byte((uint8_t *)&ret[i * 8 + 0], (uint8_t *)&ret[i * 8 + 6]);
		swap_byte((uint8_t *)&ret[i * 8 + 1], (uint8_t *)&ret[i * 8 + 7]);
		swap_byte((uint8_t *)&ret[i * 8 + 2], (uint8_t *)&ret[i * 8 + 4]);
		swap_byte((uint8_t *)&ret[i * 8 + 3], (uint8_t *)&ret[i * 8 + 5]);
	}
	return (ret);
}

char	*md5(char *tocrypt, size_t len)
{
	char *str;
	size_t padded_len;
	t_md5 algo;

	str = di_preprocessing(&padded_len, tocrypt, len, 1);
	if (str == NULL)
		return (NULL);
	algo = algorithm((uint8_t *)str, padded_len);
	free(str);
	return (md5_ret(&algo));
}
