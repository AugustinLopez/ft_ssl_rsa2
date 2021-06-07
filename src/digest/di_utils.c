/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   di_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/27 19:09:44 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/07 11:30:49 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "mylib/libft.h"

void swap_byte(uint8_t *a, uint8_t *b)
{
	if (*a != *b) {
		*a = *a ^ *b;
		*b = *b ^ *a;
		*a = *a ^ *b;
	}
}

uint32_t l_32rot(uint32_t x, size_t shift)
{
	return ((x << shift) | (x >> (32 - shift)));
}

uint32_t r_32rot(uint32_t x, size_t shift)
{
	return ((x >> shift) | (x << (32 - shift)));
}

uint64_t l_64rot(uint64_t x, size_t shift)
{
	return ((x << shift) | (x >> (64 - shift)));
}

uint64_t r_64rot(uint64_t x, size_t shift)
{
	return ((x >> shift) | (x << (64 - shift)));
}

void	*uint8to32(uint32_t *dest, uint8_t *src, size_t len, int little_endian)
{
	if (little_endian == 1) {
		for (size_t i = 0; i < len; i++)
			dest[i] = src[i * 4] + (src[i * 4 + 1] << 8) + (src[i * 4 + 2] << 16)
			+ (src[i * 4 + 3] << 24);
	}
	else {
		for (size_t i = 0; i < len; i++)
			dest[i] = (src[i * 4] << 24) + (src[i * 4 + 1] << 16)
			+ (src[i * 4 + 2] << 8) + src[i * 4 + 3];
	}
	return (dest);
}

void	*uint8to64(uint64_t *dest, uint8_t *src, size_t len, int little_endian)
{
	size_t	i;
	unsigned char	*tmp;

	(void)little_endian;
	tmp = (unsigned char *)src;
	i = 0;
	while (i < len) {
		dest[i] = ((uint64_t)tmp[i * 8] << 56)
			+ ((uint64_t)tmp[i * 8 + 1] << 48)
			+ ((uint64_t)tmp[i * 8 + 2] << 40)
			+ ((uint64_t)tmp[i * 8 + 3] << 32)
			+ ((uint64_t)tmp[i * 8 + 4] << 24)
			+ ((uint64_t)tmp[i * 8 + 5] << 16)
			+ ((uint64_t)tmp[i * 8 + 6] << 8)
			+ ((uint64_t)tmp[i * 8 + 7]);
		++i;
	}
	return (dest);
}

char *di_preprocessing(size_t *padded_len, char *tocrypt, size_t len, int little_endian)
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

char *di_lpreprocessing(size_t *padded_len, char *tocrypt, size_t len, int little_endian)
{
	char	tmp[144];
	char	*ret;
	int	pad;
	int	i;

	ft_memset(tmp, 0, 144);
	tmp[0] = 0x80;
	pad = 111 - len % 128;
	if (pad < 0)
		pad += 128;
	i = -1;
	if (little_endian == 1)
		while (++i < 8)
			tmp[pad + 8 - i] = ((len * 8) >> (56 - i * 8)) & 0xff;
	else
		while (++i < 8)
			tmp[pad + 9 + i] = ((len * 8) >> (56 - i * 8)) & 0xff;
	ret = malloc(len + pad + 17);
	if (!ret)
		return (NULL);
	ft_memcpy(ret, tocrypt, len);
	ft_memcpy(ret + len, tmp, pad + 17);
	*padded_len = len + pad + 17;
	return (ret);
}
