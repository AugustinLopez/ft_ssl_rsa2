/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decrypt_private_rsa.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/03 21:08:30 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"

static int load_rsa_salt(t_string *input, t_rsa *rsa, int i)
{
	for (int j = 0;j < 16; j++) {
		if (ft_strchr("0123456789", sptr(input)[i + j]) != NULL) {
			if (j % 2 == 0)
				rsa->salt[j / 2] = 16 * (sptr(input)[i + j] - '0');
			else
				rsa->salt[j / 2] += (sptr(input)[i + j] - '0');
		}
		else if (ft_strchr("ABCDEF", sptr(input)[i + j]) != NULL) {
			if (j % 2 == 0)
				rsa->salt[j / 2] = 16 * (10 + sptr(input)[i + j] - 'A');
			else
				rsa->salt[j / 2] += (10 + sptr(input)[i + j] - 'A');
		}
		else if (ft_strchr("abcdef", sptr(input)[i + j]) != NULL) {
			if (j % 2 == 0)
				rsa->salt[j / 2] = 16 * (10 + sptr(input)[i + j] - 'a');
			else
				rsa->salt[j / 2] += (10 + sptr(input)[i + j] - 'a');
		}
		else
			return (-1);
	}
	return (0);
}

static int readrsa_priv(char *str, size_t len, t_rsa *rsa)
{
	size_t i;
	size_t j;
	size_t imax;

	i = 0;
	(void)len;
	if (readsequence(str, &i, &imax, len) == -1)
		return (-1);
	if (ft_memcmp(str + i, "\x02\x01\x00", 3) != 0)
		return (-1);
	i += 3;
	for (j = 0; j<RSA_MEMBER_COUNT; j++){
		if (readnumber(str, &i, (rsa->member[j]), &(rsa->size[j]), len) == -1)
			return (-1);
	}
	return (0);
}

int rsa_decrypt(t_rsa *rsa, t_sslrsa *arg, size_t start, size_t end)
{
	t_string *tmp;
	t_string *input;
	int ret;

	input = arg->skey;
	if (decode_base64(&tmp, sptr(input) + start, slen(input) - end - start) == -1)
		return (-1);
	if (rsa->key_count == 1)
		des_cbc((uint8_t *)sptr(tmp), slen(tmp), rsa->nkey, rsa->salt, 1);
	else if (rsa->key_count == 3)
		des3_cbc((uint8_t *)sptr(tmp), slen(tmp), rsa->nkey, rsa->salt, 1);
	ret = 0;
	if (readrsa_priv(sptr(tmp), slen(tmp), rsa) == -1)
		ret = -1;
	sfree(tmp);
	return (ret);
}

int decode_private_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	int i;

	rsa->decrypt = 1;
	if (ft_strncmp(sptr(arg->skey) + slen(arg->skey) - 30, "-----END RSA PRIVATE KEY-----\n", 30) != 0)
		return (-1);
	if (ft_strncmp(sptr(arg->skey) + 32, "Proc-Type: 4,ENCRYPTED\nDEK-Info: DES-", 37) == 0) {
		if (ft_strncmp(sptr(arg->skey) + 69, "CBC,", 4) == 0)
			rsa->key_count = 1;
		else if (ft_strncmp(sptr(arg->skey) + 69, "EDE3-CBC,", 9) == 0)
			rsa->key_count = 3;
		else
			return (-1);
		i = rsa->key_count == 1 ? 73 : 78;
		if (load_rsa_salt(arg->skey, rsa, i) != 0)
			return (-1);
		if (ft_strncmp(sptr(arg->skey) + i + 16, "\n\n", 2) != 0)
			return (-1);
		i += 18;
		if (rsa_load_key(rsa, arg->argin, NULL) != 0)
			return (-1);
	}
	else
		i = 32;
	if (rsa_decrypt(rsa, arg, i, 30) != 0)
		return (-1);
	return (0);
}
