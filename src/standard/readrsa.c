/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   genrsa.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 01:28:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/13 17:07:11 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"
#include <errno.h>
#include <stdint.h>

t_string *genrsa(uint64_t p, uint64_t q, uint64_t e)
{
	t_string *str;
	t_string *out;
	uint64_t d;
	int ret;

	if (salloc(&str, "\x30\x00\x02\x01\x00", 5) == -1)
		return (NULL);
	d = modmulinv(e, (p - 1) * (q - 1));
	ret = 0;
	ret = ret != 0 ? -1 : addnumber(str, p * q);
	ret = ret != 0 ? -1 : addnumber(str, e);
	ret = ret != 0 ? -1 : addnumber(str, d);
	ret = ret != 0 ? -1 : addnumber(str, p);
	ret = ret != 0 ? -1 : addnumber(str, q);
	ret = ret != 0 ? -1 : addnumber(str, d % (p - 1));
	ret = ret != 0 ? -1 : addnumber(str, d % (q - 1));
	ret = ret != 0 ? -1 : addnumber(str, modmulinv(q, p));
	if (ret != 0) {
		print_err("genrsa", NULL, NULL, errno);
		return (NULL);
	}
	sptr(str)[1] = slen(str) - 2;
	ret = encode_base64(&out, sptr(str), slen(str));
	sfree(str);
	if (ret == -1) {
		print_err("genrsa", NULL, NULL, errno);
		return (NULL);
	}
	return (out);
}

static int readnumber(char *str, size_t *index, uint8_t *num, int *numsize)
{
	int imax;
	size_t j;

	if (str[*index++] != 0x02)
		return (-1);
	imax = str[*index++];
	if (imax >= 0x80) {
		j = imax - 0x80;
		if (j > 2)
			return (-1);
		imax = 0;
		while (j-- < 0)
			imax = (imax << 8) + str[*index++];
	}
	else if (imax == 0)
		return (-1);
	while (imax-- > 0) {
		if (*numzise == 0 && str[*index] == 0)
			++*index;
		else
			(*num)[*numsize++] = str[*index++];
	}
	return (0);
}

int readrsa(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;

	if (ft_strncmp(str, "-----BEGIN RSA PRIVATE KEY-----", 31) == 0) {
		i = 31;
		//decode base64 here + check end of file
		if (str + i++ != 0x30)
			return (-1);
		imax = str + i++;
		if (imax >= 0x80) {
			j = imax - 0x80;
			if (j > 2)
				return (-1); //bigger than required for 4096 bits
			imax = 0;
			while (j-- < 0)
				imax = (imax << 8) + str[i++];
		}
		if (ft_memcmp(str + i, "\x02\x01\x00", 3) != 0)
			return (-1);
		i += 3;
		for (j = 0;j<RSA_MEMBER_COUNT;j++){
			if (readnumber(str, &i, &(rsa->member[i]), &(rsa->size[i])) == -1)
				return (-1);
		}
		return (0);
	}
	return (0);
}
