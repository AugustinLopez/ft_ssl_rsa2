/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readrsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/20 15:29:15 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"

static int readrsa_privall(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;
	char type;

	(void)len;
	i = 0;
	if (readsequence(str, &i, &imax) == -1)
		return (-1);
	if (ft_memcmp(str + i, "\x02\x01\x00", 3) != 0)
		return (-1);
	i += 3;
	if (ft_memcmp(str + i, "\x30\x0d\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01\x05\x00", 15) != 0)
		return (-1);
	type = str[i + 15];
	if (type != 0x03 && type != 0x04)
		return (-1);
	i += 16;
	imax = (uint8_t)str[i++];
	if (imax >= 0x80) {
		j = imax - 0x80;
		if (j > 2)
			return (-1);
		imax = 0;
		while (j-- > 0)
			imax = (imax << 8) + (uint8_t)str[i++];
	}
	if (type == 0x03 && str[i++] != 0x00)
		return (-1);
	if (readsequence(str, &i, &imax) == -1)
		return (-1);
	if (ft_memcmp(str + i, "\x02\x01\x00", 3) != 0)
		return (-1);
	i += 3;
	for (j = 0; j<RSA_MEMBER_COUNT; j++){
		if (readnumber(str, &i, (rsa->member[j]), &(rsa->size[j])) == -1)
			return (-1);
	}
	return (0);
}

static int rsa_read_private(t_rsa *rsa, t_sslrsa *arg, size_t start, size_t end)
{
	t_string *tmp;
	t_string *input;
	int ret;

	input = arg->sin;
	if (decode_base64(&tmp, sptr(input) + start, slen(input) - end - start) == -1)
		return (-1);
	ret = 0;
	if (readrsa_privall(sptr(tmp), slen(tmp), rsa) == -1)
		ret = -1;
	sfree(tmp);
	return (ret);
}

int decode_private(t_sslrsa *arg, t_rsa *rsa)
{
	rsa->decrypt = 1;
	if (ft_strncmp(sptr(arg->sin) + slen(arg->sin) - 26, "-----END PRIVATE KEY-----\n", 26) != 0)
		return (-1);
	if (rsa_read_private(rsa, arg, 28, 26) != 0)
		return (-1);
	return (0);
}
