/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readrsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/16 19:04:33 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"

static int readsequence(char *str, size_t *index, size_t *len)
{
	size_t j;

	if (str[(*index)++] != 0x30)
		return (-1);
	*len = (uint8_t)str[(*index)++];
	if (*len >= 0x80) {
		j = *len - 0x80;
		if (j > 2) //enough for 65536 bytes > more than enough for 4096 bits key
			return (-1);
		*len = 0;
		while (j-- > 0)
			*len = (*len << 8) + (uint8_t)str[(*index)++];
	}
	return (0);
}

static int readnumber(char *str, size_t *index, uint8_t *num, int *numsize)
{
	int imax;
	size_t j;

	if (str[(*index)++] != 0x02)
		return (-1);
	imax = (uint8_t)str[(*index)++];
	if (imax >= 0x80) {
		j = imax - 0x80;
		if (j > 2) //enough for 65536 bytes, more than enough for 4096 bits key.
			return (-1);
		imax = 0;
		while (j-- > 0)
			imax = (imax << 8) + (uint8_t)str[(*index)++];
	}
	else if (imax == 0)
		return (-1);
	while (imax-- > 0) {
		if (*numsize == 0 && str[*index] == 0)
			++*index;
		else
			num[(*numsize)++] = (uint8_t)str[(*index)++];
	}
	return (0);
}

static int readrsa_priv(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;

	i = 0;
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

static int readrsa_privall(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;
	char type;

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


static int readrsa_pub(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;

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


int decode_64rsa(t_string **key, int pubin, t_rsa *rsa)
{
	t_string *replacement;
	int ret;
	int i;

	ret = 0;
	//need to handle encryption
	if (pubin == 0) {
		if (ft_strncmp(sptr(*key), "-----BEGIN RSA PRIVATE KEY-----\n", 32) == 0) {
			if (ft_strncmp(sptr(*key) + slen(*key) - 30, "-----END RSA PRIVATE KEY-----", 29) != 0)
				return (-1);
			if (ft_strncmp(sptr(*key) + 32, "Proc-Type: 4,ENCRYPTED\nDEK-Info: DES-", 37) == 0) {
				if (ft_strncmp(sptr(*key) + 69, "CBC," 4) == 0) {
					rsa->key = 1;
					for (int j = 0;j < 16; i++) {
						if (ft_strchr(sptr(*key)[73 + j], "0123456789") != NULL) {
							if (j % 2 == 0)
								rsa->salt[j / 2] = 16 * (sptr(*key)[73 + j] - '0');
							else
								rsa->salt[j / 2] += (sptr(*key)[73 + j] - '0');
						}
						else if (ft_strchr(sptr(*key)[73 + j], "abcdef") != NULL) {
							if (j % 2 == 0)
								rsa->salt[j / 2] = 16 * (10 + sptr(*key)[73 + j] - 'a');
							else
								rsa->salt[j / 2] += (10 + sptr(*key)[73 + j] - 'a');
						}
						else
							return (-1);
					}
				}
				else
					return (-1);

			}
			if (decode_base64(&replacement, sptr(*key) + 32, slen(*key) - 63) == -1)
				return (-1);
			if (readrsa_priv(sptr(replacement), slen(replacement), rsa) == -1)
				ret = -1;
		}
		else if (ft_strncmp(sptr(*key), "-----BEGIN PRIVATE KEY-----\n", 28) == 0) {
			if (ft_strncmp(sptr(*key) + slen(*key) - 26, "-----END PRIVATE KEY-----", 25) != 0)
				return (-1);
			if (decode_base64(&replacement, sptr(*key) + 28, slen(*key) - 55) == -1)
				return (-1);
			if (readrsa_privall(sptr(replacement), slen(replacement), rsa) == -1)
				ret = -1;
		}
		else
			return (-1);
	}
	//need to adjust the number
	else if (pubin == 1) {
		if (ft_strncmp(sptr(*key), "-----BEGIN RSA PUBLIC KEY-----\n", 31) == 0) {
			if (ft_strncmp(sptr(*key) + slen(*key) - 29, "-----END RSA PUBLIC KEY-----", 28) != 0)
				return (-1);
			printf("%.*s\n", slen(*key) - 59, sptr(*key) + 30);
			if (decode_base64(&replacement, sptr(*key) + 30, slen(*key) - 59) == -1)
				return (-1);
			if (readrsa_pub(sptr(replacement), slen(replacement), rsa) == -1)
				ret = -1;
		}
		else if (ft_strncmp(sptr(*key), "-----BEGIN PUBLIC KEY-----\n", 27) == 0) {
			if (ft_strncmp(sptr(*key) + slen(*key) - 25, "-----END PUBLIC KEY-----", 24) != 0)
				return (-1);
			printf("%.*s\n", slen(*key) - 51, sptr(*key) + 26);
			if (decode_base64(&replacement, sptr(*key) + 26, slen(*key) - 51) == -1)
				return (-1);
			if (readrsa_puball(sptr(replacement), slen(replacement), rsa) == -1)
				ret = -1;
		}
		else
			return (-1);
	}
	else
		return (-1);
	sdel(&replacement);
	return (ret);
}
