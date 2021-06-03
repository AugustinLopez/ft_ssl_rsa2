/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encode_b64.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 11:47:35 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/03 17:28:27 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_string.h"
#include <stdint.h>
#include <stddef.h>

static uint8_t	index_to_base64(unsigned int c)
{
	if (c <= 25)
		return ('A' + c);
	if (c <= 51)
		return ('a' + c - 26);
	if (c <= 61)
		return ('0' + c - 52);
	if (c == 62)
		return ('+');
	if (c == 63)
		return ('/');
	return (0);
}

static void	perform_encoding_b64(uint8_t *src, uint8_t *dest, size_t len)
{
	size_t	d;
	size_t	s;

	d = 0;
	s = 0;
	while (s < len)
	{
		dest[d] = index_to_base64((src[s] & 0xfc) >> 2);
		dest[d + 1] = index_to_base64((src[s] & 0x3) << 4);
		if (s + 1 >= len)
			break ;
		dest[d + 1] = index_to_base64(((src[s] & 0x3) << 4)
		| ((src[s + 1] & 0xf0) >> 4));
		dest[d + 2] = index_to_base64((src[s + 1] & 0xf) << 2);
		if (s + 2 >= len)
			break ;
		dest[d + 2] = index_to_base64(((src[s + 1] & 0xf) << 2)
		| ((src[s + 2] & 0xc0) >> 6));
		dest[d + 3] = index_to_base64(src[s + 2] & 0x3f);
		s += 3;
		d += 4;
	}
}

int		encode_base64(t_string **str, char *src, size_t len)
{
	if (salloc(str, NULL, 0) == -1)
		return (-1);
	if (len == 0)
		return (0);
	for (size_t i = (len - 1) / 3 + 1; i > 0; i--) {
		if (satrail(*str, "====", 4) == -1)
			return (-1);
	}
	perform_encoding_b64((uint8_t *)src, (uint8_t *)sptr(*str), slen(*str));
	return (0);
}
