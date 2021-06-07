/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pbkdf_and_key_des.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/08 13:58:33 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/18 13:11:43 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include <stdio.h>
#include "cypher.h"
#include "digest/md5.h"
#include <stdlib.h>
#include <string.h>

int		key_provided(t_cypher *arg)
{
	char	tmp[64];
	int		i;

	//load the keys
	bzero(tmp, 64);
	strncpy((char *)tmp, arg->av_key, 48);
	if (arg->option & SSL_CYPHER_3KEY) {
		i = -1;
		while (++i < 3) {
			if (load_to_hex64((uint8_t *)arg->nkey[i], tmp + 16 * i, "key") < 0) {
				return (-1);
			}
		}
	}
	else if (load_to_hex64((uint8_t *)arg->nkey[0], tmp, "key") == -1) {
		return (-1);
	}

	//load the vector only if necessary
	if (!(arg->option & SSL_CYPHER_IV)) {
		return (0);
	}
	
	//if the key is provided, the vector must be provided
	if (arg->av_vector == NULL) {
		fprintf(stderr, "iv undefined\n");
		return (-1);
	}
	return (load_to_hex64((uint8_t *)arg->vector, arg->av_vector, "vector"));
}

int		convert_pbkdf(t_cypher *arg, char *src, int key)
{
	int	i;

	//load the keys
	i = -1;
	while (++i < key) {
		if (load_to_hex64((uint8_t *)arg->nkey[i], src + 16 * i, "key") == -1) {
			return (-1);
		}
	}

	//load the vector only if necessary
	if (!(arg->option & SSL_CYPHER_IV)) {
		return (0);
	}

	//load the IV if required. It can be built from the pbkdf if not provided.
	if (arg->av_vector == NULL) {
		return (load_to_hex64((uint8_t *)arg->vector, src + 16 * i, "vector"));
	}
	return (load_to_hex64((uint8_t *)arg->vector, arg->av_vector, "vector"));
}

char		*pbkdf(uint8_t *pwd, size_t len, uint8_t *salt, int key)
{
	char		*tmp;
	char		*tmp2;
	char		*tmp3;
	
	//1 key: pbkdf is MD5(PWD | SALT);
	memcpy(salt, pwd + len, 8);
	tmp = md5(pwd, len + 8);

	//We need 16 bytes per key + 16 bytes for the IV. MD5 is 32 bytes.
	if (tmp == NULL || key < 3 || strlen(tmp) >= 64) {
		return (tmp);
	}

	//3 keys: pbkdf is MD5((MD5(PWD | SALT)) | PWD | SALT);
	// Note that 1st MD5 is used as an HEX value
	memmove(pwd + 16, pwd, len + 8);
	bzero(pwd, 16);
	if (c16_to_hex64(pwd, tmp) == -1 || c16_to_hex64(pwd + 8, tmp + 16) == -1) {
		free(tmp);
		return (NULL);
	}
	tmp2 = md5(pwd, len + 8 + 16);
	if (tmp2 == NULL) {
		free(tmp);
		return (NULL);
	}
	asprintf(&tmp3, "%s%s", tmp, tmp2);
	free(tmp);
	free(tmp2);
	return (tmp3);
}
