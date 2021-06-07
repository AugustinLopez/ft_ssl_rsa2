/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encode_io_des.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/08 13:58:33 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/18 13:10:19 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cypher.h"
#include "common/ssl_string.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

static int	pkcs7_padding(t_string *topad)
{
	//N bytes of value N are added to obtain a length multiple of 8
	//Padding always occurs even if the original length is a multiple of 8
	char	val;
	int		len;

	val  = 8 - topad->len % 8;
	len = val;
	while (len--) {
		*topad = plus_string(*topad, &val, 1);
		if (topad->err != 0) {
			return (-1);
		}
	}
	return (0);
}

static int	encode_load_string(t_cypher *arg, t_string *input)
{
	//print the salt if a password is used without base64 nor output file.
	if (arg->av_key == NULL && arg->av_output == NULL
	&& !(arg->option & SSL_CYPHER_BASE64)) {
		printf("Salted__%.8s", arg->salt);
		fflush(stdout);
	}

	//load the input in av_input (or stdin if NULL).
	*input = cypher_input(arg->av_input);
	if (input->err != 0) {
		return (-1);
	}

	//some DES algorithm requires a specific padding.
	if (!(arg->option & SSL_CYPHER_NOPAD)) {
		return (pkcs7_padding(input));
	}
	return (0);
}

static int	encode_pbkdf(t_cypher *arg, char *algo)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1];
	char	*tmp;
	size_t	len;
	int		n;

	//password is either provided or prompted
	if (load_password(pwd, arg->av_pass, arg->option, algo) == -1) {
		return (-1);
	}
	len = strlen(pwd);

	//salt is either provided or random
	if (load_to_hex64((uint8_t *)(pwd + len), arg->av_salt, "salt") == -1) {
		return (-1);
	}
	n = 1;
	if (arg->option & SSL_CYPHER_3KEY) {
		n = 3;
	}

	//Same algorithm as default openssl to convert pass and salt.
	tmp = pbkdf((uint8_t *)pwd, len, arg->salt, n);
	if (tmp == NULL) {
		return (-1);
	}

	//Put the result in our key(s) and IV (if not provided/required).
	n = convert_pbkdf(arg, tmp, n);
	free(tmp);
	return (n);
}

int		encode_io(t_cypher *arg, t_string *input, char *algo)
{
	//load key from provided argument. If IV required, it must be provided.
	if (arg->av_key != NULL) {
		if (key_provided(arg) < 0) {
			return (-1);
		}
	}

	//build key from password and salt. Everything is built if not provided.
	else if (encode_pbkdf(arg, algo) < 0) {
		return (-1);
	}
	
	//if the INFO flag is activated, no further input is needed.
	if (arg->option & SSL_CYPHER_INFO) {
		return (0);
	}

	//Otherwise, load the text to encode
	return (encode_load_string(arg, input));
}
