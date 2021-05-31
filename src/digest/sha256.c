/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 11:55:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/18 13:12:40 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include "digest/sha2.h"
#include "common/ssl_string.h"
#include <stdio.h>

char			*sha256(uint8_t *tocrypt, size_t len)
{
	t_string	buffer;
	t_sha2xx	algo;
	char		*re;

	buffer = dig_preprocessing(tocrypt, len, 0);
	if (buffer.err)
		return (NULL);
	algo = sha2xx_algorithm(buffer.s, len, 256);
	free_string(&buffer);
	asprintf(&re, "%08x%08x%08x%08x%08x%08x%08x%08x", algo.h[0], algo.h[1]
	, algo.h[2], algo.h[3], algo.h[4], algo.h[5], algo.h[6], algo.h[7]);
	return (re);
}

char			*sha224(uint8_t *tocrypt, size_t len)
{
	t_string	buffer;
	t_sha2xx	algo;
	char		*re;

	buffer = dig_preprocessing(tocrypt, len, 0);
	if (buffer.err)
		return (NULL);
	algo = sha2xx_algorithm(buffer.s, len, 224);
	free_string(&buffer);
	asprintf(&re, "%08x%08x%08x%08x%08x%08x%08x", algo.h[0], algo.h[1]
	, algo.h[2], algo.h[3], algo.h[4], algo.h[5], algo.h[6]);
	return (re);
}
