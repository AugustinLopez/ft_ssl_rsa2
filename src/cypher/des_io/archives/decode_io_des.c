/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decode_io_des.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 13:53:17 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/18 13:09:36 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cypher.h"
#include "common/ssl_string.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

static int	decode_crypted_text(t_cypher *arg, t_string *input)
{
	t_string	tmp;

	//If flag base64 is provided, decode the input
	if (arg->option & SSL_CYPHER_BASE64) {
		tmp = cypher_input(arg->av_input);
		if (tmp.err != 0) {
			return (-1);
		}
		*input = decode_base64(tmp.s, tmp.len);
		free_string(&tmp);
		if (input->err != 0) {
			return (-1);
		}
	}
	else {
		*input = cypher_input(arg->av_input);
		if (input->err != 0) {
			return (-1);
		}
	}
	return (0);
}

static int	decode_crypted_salt(t_cypher *arg, t_string *input, char *pwd
			, size_t len)
{
	//When a password is provided, the following start is expected
	//start value + actual salt value is at least 16 bytes.
	if (memcmp(input->s, "Salted__", 8) != 0 || input->len < 16)
	{
		free_string(input);
		fprintf(stderr, "bad magic number\n");
		return (-1);
	}
	memcpy(arg->salt, input->s + 8, 8);
	memcpy((uint8_t *)pwd + len, input->s + 8, 8);

	//We no longer need those 16 bytes. Removing them make things easier after.
	*input = reduce_string(*input, 16, 0);
	if (input->err != 0)
		return (-1);
	return (0);
}
static int		decode_pbkdf(t_cypher *arg, t_string *input, char *algo)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1];
	char	*tmp;
	size_t	len;
	int		n;

	//password is either provided or prompted
	if (load_password((char *)pwd, arg->av_pass, arg->option, algo) == -1) {
		return (-1);
	}

	//text must be loaded to obtain the salt. Base64 flag might be active.
	if (decode_crypted_text(arg, input) == -1) {
		return (-1);
	}

	//When decoding, the salt is always taken from the input.
	len = strlen(pwd);
	if (decode_crypted_salt(arg, input, pwd, len) == -1) {
		return (-1);
	}
	n = 1;
	if (arg->option & SSL_CYPHER_3KEY) {
		n = 3;
	}

	//Same algorithm as default openssl to convert pass and salt
	tmp = pbkdf((uint8_t *)pwd, len, arg->salt, n);
	if (tmp == NULL) {
		return (-1);
	}

	//Put the result in our key(s) and IV (if not provided/required).
	n = convert_pbkdf(arg, tmp, n);
	free(tmp);
	return (n);
}

int		decode_io(t_cypher *arg, t_string *input, char *algo)
{
	//build key from password. Salt is taken from the input. Everything else
	//is built if not provided.
	if (arg->av_key == NULL) {
		if (decode_pbkdf(arg, input, algo) == -1) {
			return (-1);
		}
		return (0);
	}

	//load key from provided argument. If IV is required, it must be provided.
	if (key_provided(arg) == -1) {
		return (-1);
	}

	//if the info FLAG is active with keys provided, no further input is needed.
	if (arg->option & SSL_CYPHER_INFO) {
		return (0);
	}

	//Otherwise, load the text to decode. Base64 flag might be active
	return (decode_crypted_text(arg, input));
}
