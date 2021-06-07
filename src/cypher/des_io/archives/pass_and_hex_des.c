/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass_and_hex_des.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 13:53:17 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/18 13:08:36 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "cypher.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

static int	char_to_hex(char c)
{
	if ('0' <= c && c <= '9')
		return (c - '0');
	else if ('a' <= c && c <= 'f')
		return (c - 'a' + 10);
	else if ('A' <= c && c <= 'F')
		return (c - 'A' + 10);
	else
		return (-1);
}

int		c16_to_hex64(uint8_t *hex, char *src)
{
	uint8_t	tmp[16];
	int		right;
	int		left;
	size_t	len;

	//Up to 16 characters are read. If less than 16 are read, the rest is filled
	//with '0'. Characters must be valid hexadecimal digit.
	len = 0;
	memset((void *)tmp, '0', 16);
	while (*(src + len) != 0 && len < 16)
	{
		if (strchr("0123456789abcdefABCDEF", *(src + len)) == NULL)
			return (-1);
		tmp[len] = src[len];
		 ++len;
	}
	len = 0;
	while (len < 8)
	{
		left = char_to_hex(tmp[len * 2]);
		right = char_to_hex(tmp[len * 2 + 1]);
		hex[len++] = left * 16 + right;
	}
	return (0);
}

int			load_password(char *dest, char *src, uint8_t option, char *algo)
{
	char	ver[_SC_PASS_MAX + 1];
	char	prompt[128];

	bzero((void *)dest, _SC_PASS_MAX + 1);
	//Copy in dest if the pwd is provided as argument
	if (src != NULL) {
		strncpy(dest, src, _SC_PASS_MAX);
		return (0);
	}

	bzero((void *)ver, _SC_PASS_MAX + 1);
	bzero((void *)prompt, 128);
	//Only one password is required when decrypting
	if (option & SSL_CYPHER_DECRYPT) {
		snprintf((char *)prompt, 128, "enter %s decryption password:", algo);
		strncpy(dest, getpass(prompt), _SC_PASS_MAX);
		if (dest[0] == '\0') {
			fprintf(stderr, "bad password read\n");
			return (-1);
		}
		return (0);
	}

	//Otherwise, we ask the user to enter the password twice
	snprintf((char *)prompt, 128, "Verifying - enter %s encryption password:", algo);
	strncpy(dest, getpass(prompt + 12), _SC_PASS_MAX);
	if (dest[0] != '\0') {
		strncpy(ver, getpass(prompt), _SC_PASS_MAX);
	}
	if (dest[0] == '\0' || ver[0] == '\0') {
		fprintf(stderr, "bad password read\n");
		return (-1);
	}
	else if (strcmp(dest, ver) != 0) {
		fprintf(stderr, "Verify failure\nbad password read\n");
		return (-1);
	}
	return (0);
}

int			load_to_hex64(uint8_t *dest, char *src, char *mod)
{
	int	i;

	//If no salt is provided, random values can be assigned
	//Otherwise, argument must be provided & valid
	if (src == NULL && (memcmp(mod, "salt", 3) == 0)) {
		i = -1;
		while (++i < 8) {
			dest[i] = (uint8_t)ft_rand2();
		}
	}
	else if (src == NULL || c16_to_hex64(dest, src) == -1) {
		fprintf(stderr, "non-hex digit\ninvalid hex %s value\n", mod);
		return (-1);
	}
	return (0);
}
