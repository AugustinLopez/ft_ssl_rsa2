/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readrsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/20 13:55:12 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "mylib/libft.h"
#include "ft_cypher.h"
#include "st_cmd.h"

int readsequence(char *str, size_t *index, size_t *len)
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

int readnumber(char *str, size_t *index, uint8_t *num, int *numsize)
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

int rsa_load_key(t_rsa *rsa, char *pass)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1]; //+ salt + some place for 3key
	char	*digest;

	ft_memset(pwd, 0, _SC_PASS_MAX + 1);
	if (pass != NULL)
		ft_strncpy(pwd, pass, _SC_PASS_MAX);
	else if (load_password(pwd, rsa->decrypt, "PEM") == -1) {
		write(STDERR_FILENO, "bad password read\n", 18);
		return (-1);
	}
	if (rsa->decrypt == 0) {
		for (int i = 0; i < 8; i ++)
			rsa->salt[i] = (uint8_t)ft_rand();
	}
	digest = pbkdf((uint8_t *)pwd, ft_strlen(pwd), rsa->salt, rsa->key_count);
	if (digest == NULL)
		return (-1);
	for (int i = 0; i < rsa->key_count; i++) {
		if (c16_to_hex64(rsa->nkey + i * 8, digest + i * 16) == -1) {
			free(digest);
			write(STDERR_FILENO, "non-hex_digit\ninvalid hex key value\n", 36);
			return (-1);
		}
	}
	free(digest);
	return (0);
}
