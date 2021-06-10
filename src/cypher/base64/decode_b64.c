/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decode_b64.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 11:47:35 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 18:00:40 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include "mylib/ft_string.h"
#include "mylib/libft.h"

static int8_t	base64_to_index(int8_t c)
{
	if ('A' <= c && c <= 'Z')
		return (c - 'A');
	if ('a' <= c && c <= 'z')
		return (c - 'a' + 26);
	if ('0' <= c && c <= '9')
		return (c - '0' + 52);
	if (c == '+')
		return (62);
	if (c == '/')
		return (63);
	return (-1);
}

static int		read4_from_b64(uint8_t *input, int8_t *output, size_t *index
				, size_t len)
{
	int	nbr_read;

	nbr_read = 0;
	ft_memset(output, 0, 4 * sizeof(*output));
	while (nbr_read < 4)
	{
		while (*index < len && (input[*index] == ' ' || input[*index] == '\t'
			|| input[*index] == '\n' || input[*index] == '\v'))
				++(*index);
		if (*index >= len)
			return (nbr_read);
		output[nbr_read++] = input[(*index)++];
	}
	return (4);
}

static int		first_letter(t_string *ret, int8_t *buf)
{
	uint8_t	letter;

	if (buf[0] == '=' || buf[0] == '\0')
		return (1);
	else if (buf[1] == '=' || buf[1] == '\0') {
		print_err("base64", 0, "Invalid padding in input stream", 0);
		return (-1);
	}
	buf[0] = base64_to_index(buf[0]);
	buf[1] = base64_to_index(buf[1]);
	if (buf[0] == -1 || buf[1] == -1) {
		print_err("base64", 0, "Invalid character in input stream", 0);
		return (-1);
	}
	letter = (buf[0] << 2) | ((buf[1] & 0x30) >> 4);
	if (satrail(ret, (char *)&letter, 1) == -1) {
		print_err("base64", 0, 0, errno);
		return (-1);
	}
	return (0);
}

static int		nt_letter(int i, t_string *ret, int8_t *buf)
{
	uint8_t	letter;

	if (buf[i] == '=' || buf[i] == '\0')
		return (1);
	buf[i] = base64_to_index(buf[i]);
	if (buf[i] == -1) {
		print_err("base64", 0, "Invalid character in input stream", 0);
		return (-1);
	}
	if (i == 2)
		letter = ((buf[1] & 0xf) << 4) | ((buf[2] & 0x3c) >> 2);
	else if (i == 3)
		letter = ((buf[2] & 0x3) << 6) | buf[3];
	if (satrail(ret, (char *)&letter, 1) == -1) {
		print_err("base64", 0, 0, errno);
		return (-1);
	}
	return (0);
}

int		decode_base64(t_string **str, char *src, size_t len)
{
	int8_t		buf[4];
	size_t		index;
	int			ret = 0;
	int			nbr_read;

	if (salloc(str, NULL, 0) == -1)
		return (-1);
	if (len == 0)
		return (0);
	index = 0;
	nbr_read = read4_from_b64((uint8_t *)src, (int8_t *)buf, &index, len);
	while (nbr_read > 0)
	{
		ret = ret != 0 ? ret : first_letter(*str, (int8_t *)buf);
		ret = ret != 0 ? ret : nt_letter(2, *str, (int8_t *)buf);
		ret = ret != 0 ? ret : nt_letter(3, *str, (int8_t *)buf);
		if (ret != 0)
			break ;
		nbr_read = read4_from_b64((uint8_t *)src, (int8_t *)buf, &index, len);
	}
	if (ret == -1)
		*str = NULL;
	return (ret);
}
