/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_di.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/21 15:52:36 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/15 18:00:28 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "digest.h"
# include "common/ssl_string.h"
# include <stdint.h>
# include <stddef.h>
# include <string.h>
# include <stdlib.h>

t_digest	add_elem(t_digest lst, char *add, int is_file)
{
	t_list_dig	*elem;

	if (add == NULL)
	{
		lst.err = SSL_DIGEST_NULL;
		return (lst);
	}
	elem = (t_list_dig *)malloc(sizeof(t_list_dig));
	if (elem == NULL)
	{
		lst.err = SSL_DIGEST_ERRNO;
		return (lst);
	}
	elem->next = NULL;
	elem->str = add;
	elem->is_file = is_file;
	if (lst.end == NULL)
		lst.begin = elem;
	else
		lst.end->next = elem;
	lst.end = elem;
	return (lst);
}

void		free_elem(t_digest *lst)
{
	t_list_dig	*elem;
	t_list_dig	*tmp;

	elem = lst->begin;
	while (elem)
	{
		tmp = elem;
		elem = elem->next;
		free(tmp);
		tmp = NULL;
	}
}

t_string	dig_preprocessing(uint8_t *tocrypt, size_t len, int little_endian)
{
	uint8_t		tmp[72];
	t_string	ret;
	int			pad;
	int			i;

	bzero(tmp, 72);
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
	ret = init_string();
	ret = plus_string(ret, tocrypt, len);
	ret = plus_string(ret, (uint8_t *)tmp, pad + 9);
	return (ret);
}

t_string	dig_lpreprocessing(uint8_t *tocrypt, size_t len, int little_endian)
{
	uint8_t		tmp[144];
	t_string	ret;
	int			pad;
	int			i;

	bzero(tmp, 144);
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
	ret = init_string();
	ret = plus_string(ret, tocrypt, len);
	ret = plus_string(ret, (uint8_t *)tmp, pad + 17);
	return (ret);
}

void	*uint8to64(uint64_t *dest, uint8_t *src, size_t len, int little_endian)
{
	size_t			i;
	unsigned char	*tmp;

	(void)little_endian;
	tmp = (unsigned char *)src;
	i = 0;
	while (i < len)
	{
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

void	*uint8to32(uint32_t *dest, uint8_t *src, size_t len, int little_endian)
{
	size_t	i;

	i = 0;
	if (little_endian == 1)
	{
		while (i < len)
		{
			dest[i] = src[i * 4] + (src[i * 4 + 1] << 8)
				+ (src[i * 4 + 2] << 16) + (src[i * 4 + 3] << 24);
			++i;
		}
	}
	else
	{
		while (i < len)
		{
			dest[i] = (src[i * 4] << 24) + (src[i * 4 + 1] << 16)
				+ (src[i * 4 + 2] << 8) + src[i * 4 + 3];
			++i;
		}
	}
	return (dest);
}
