/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readrsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/30 10:54:07 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"

static int readrsa_pub(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;

	(void)len;
	i = 0;
	if (readsequence(str, &i, &imax) == -1)
		return (-1);
	for (j = 0; j< 2; j++){
		if (readnumber(str, &i, (rsa->member[j]), &(rsa->size[j])) == -1)
			return (-1);
	}
	return (0);
}

static int readrsa_puball(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;
	char type;

	(void)len;
	i = 0;
	if (readsequence(str, &i, &imax) == -1)
		return (-1);
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
	for (j = 0; j< 2; j++){
		if (readnumber(str, &i, (rsa->member[j]), &(rsa->size[j])) == -1)
			return (-1);
	}
	return (0);
}

int decode_public(t_sslrsa *arg, t_rsa *rsa)
{
	t_string *replacement;
	int ret;

	if (ft_strncmp(sptr(arg->skey), "-----END PUBLIC KEY-----\n", 25) == 0)
		return (-1);
	if (decode_base64(&replacement, sptr(arg->skey) + 27, slen(arg->skey) - 52) == -1)
		return (-1);
	ret = 0;
	if (readrsa_puball(sptr(replacement), slen(replacement), rsa) == -1)
		ret = -1;
	sfree(replacement);
	return (ret);
}

int decode_public_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	t_string *replacement;
	int ret;

	if (ft_strncmp(sptr(arg->skey), "-----END RSA PUBLIC KEY-----\n", 29) == 0)
		return (-1);
	if (decode_base64(&replacement, sptr(arg->skey) + 31, slen(arg->skey) - 60) == -1)
		return (-1);
	ret = 0;
	if (readrsa_pub(sptr(replacement), slen(replacement), rsa) == -1)
		ret = -1;
	sfree(replacement);
	return (ret);
}


int decode_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	if (arg->pubin == 0) {
		if (ft_strncmp(sptr(arg->skey), "-----BEGIN RSA PRIVATE KEY-----\n", 32) == 0)
			return (decode_private_rsa(arg, rsa));
		else if (ft_strncmp(sptr(arg->skey), "-----BEGIN PRIVATE KEY-----\n", 28) == 0)
			return (decode_private(arg, rsa));
		else if (ft_strncmp(sptr(arg->skey), "-----BEGIN ENCRYPTED PRIVATE KEY-----\n", 38) == 0)
			print_err(NULL, NULL, "encrypted private key not supported", 0);
	}
	else if (arg->pubin == 1) {
		if (ft_strncmp(sptr(arg->skey), "-----BEGIN RSA PUBLIC KEY-----\n", 31) == 0)
			return (decode_public_rsa(arg, rsa));
		else if (ft_strncmp(sptr(arg->skey), "-----BEGIN PUBLIC KEY-----\n", 27) == 0)
			return (decode_public(arg, rsa));
	}
	return (-1);
}
