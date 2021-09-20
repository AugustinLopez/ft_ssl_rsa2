/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readrsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/20 16:37:33 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"

static int load_rsa_salt(t_string *input, t_rsa *rsa, size_t *index)
{
	size_t imax;
	uint8_t val;

	if (readsequence(sptr(input), index, &imax) == -1)
		return (-1);
	if (readsequence(sptr(input), index, &imax) == -1)
		return (-1);
	if (ft_memcmp(sptr(input) + *index, "\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x05\x03", 11) != 0)
		return (-1);
	rsa->key_count = 1;
	*index += 11;
	if (readsequence(sptr(input), index, &imax) == -1)
		return (-1);
	if (ft_memcmp(sptr(input) + *index, "\x04\x08", 2) != 0)
		return (-1);
	*index += 2;
	for (int j = 0; j < 8;j++) {
		rsa->salt[j] = sptr(input)[(*index)++];
		printf("%02x\n", rsa->salt[j]);
	}
	if (ft_memcmp(sptr(input) + *index, "\x02\x01\x01\x04", 4) != 0)
		return (-1);
	*index += 4;
	val = (uint8_t)sptr(input)[*index];
	if (val > 0x82 || val == 0x00 || val == 0x80)
		return (-1);
	else if (val < 0x80)
		*index += 1;
	else
		*index += 1 + val - 0x80;
	printf("%02x%02x\n", sptr(input)[*index],sptr(input)[*index+1]);
	return (0);
}

int decode_encrypted(t_sslrsa *arg, t_rsa *rsa)
{
	size_t i;
	size_t imax;
	t_string *tmp;
	t_string *bla;

	rsa->decrypt = 1;
	if (ft_strncmp(sptr(arg->sin) + slen(arg->sin) - 36, "-----END ENCRYPTED PRIVATE KEY-----\n", 36) != 0)
		return (-1);
	i = 38;
	if (decode_base64(&tmp, sptr(arg->sin) + i, slen(arg->sin) - i - 36) == -1)
		return (-1);
	i = 0;
	if (load_rsa_salt(tmp, rsa, &i) == -1) {
		sfree(tmp);
		return (-1);
	}
	if (rsa_load_key(rsa, arg->argin) != 0) {
		sfree(tmp);
		return (-1);
	}
	des_cbc((uint8_t *)sptr(tmp) + i, slen(tmp) - i, rsa->nkey, rsa->salt, 1);
	printf("%02x\n", (uint8_t)(sptr(tmp) + i));
	i = 0;
	if (readsequence(sptr(tmp), &i, &imax) == -1) {
		sfree(tmp);
		return (-1);
	}
	if (ft_memcmp(sptr(tmp) + i, "\x02\x01\x00", 3) != 0) {
		sfree(tmp);
		return (-1);
	}
	i += 3;
	for (int j = 0; j<RSA_MEMBER_COUNT; j++) {
		if (readnumber(sptr(tmp), &i, (rsa->member[j]), &(rsa->size[j])) == -1) {
			sfree(tmp);
			return (-1);
		}
	}
	sfree(tmp);
	return (0);
}
