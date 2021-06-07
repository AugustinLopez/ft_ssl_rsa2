/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_des.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 19:13:26 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/15 12:00:53 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ssl.h"
#include "cypher.h"
#include "common/ssl_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int	add_salt_if_required(t_cypher arg, t_string *input)
{
	char	buf[16];

	if (arg.option & SSL_CYPHER_DECRYPT || arg.av_key != NULL)
		return (0);
	if (arg.av_output == NULL && !(arg.option & SSL_CYPHER_BASE64))
		return (0);
	memcpy(buf, "Salted__", 8);
	memcpy(buf + 8, arg.salt, 8);
	//salt is added to the left
	*input = before_string(*input, buf, 16);
	if (input->err != 0)
		return (-1);
	return (0);
}

static int	valid_output(t_string *input)
{
	uint8_t	ret;
	int		i;

	if (input->len == 0)
		ret = 0;
	else
		ret = (uint8_t)(input->s)[input->len - 1];
	if (ret < 1 || ret > 16 || input->len < ret) {
		fprintf(stderr, "bad magic number\n");
		free_string(input);
		return (-1);
	}
	i = 0;
	while (++i < ret) {
		if ((input->s)[input->len - 1 - i] != ret) {
			fprintf(stderr, "bad magic number\n");
			free_string(input);
			return (-1);
		}
	}

	//padding must be removed from the right before output the decrypted text
	*input = reduce_string(*input, ret, 1);
	if (input->err != 0) {
		return (-1);
	}
	return (0);
}

static void	des_print_info(t_cypher arg)
{
	int	n;
	int	i;

	if (arg.av_key == NULL) {
		printf("salt=%02X%02X%02X%02X%02X%02X%02X%02X\n", arg.salt[0]
		, arg.salt[1], arg.salt[2], arg.salt[3], arg.salt[4], arg.salt[5]
		, arg.salt[6], arg.salt[7]);
	}
	n = 1 + 2 * ((arg.option & SSL_CYPHER_3KEY) != 0);
	i = -1;
	printf("key=");
	while (++i < n) {
		printf("%02X%02X%02X%02X%02X%02X%02X%02X", arg.nkey[i][0]
		, arg.nkey[i][1], arg.nkey[i][2], arg.nkey[i][3], arg.nkey[i][4]
		, arg.nkey[i][5], arg.nkey[i][6], arg.nkey[i][7]);
	}
	printf("\n");
	if (arg.option & SSL_CYPHER_IV) {
		printf("iv =%02X%02X%02X%02X%02X%02X%02X%02X\n", arg.vector[0]
		, arg.vector[1], arg.vector[2], arg.vector[3] , arg.vector[4]
		, arg.vector[5], arg.vector[6], arg.vector[7]);
	}
}

int			des_output(t_cypher arg, t_string *input)
{
	t_string	output;
	int			ret;

	//salt must be added when output is encoded with a password.
	//If output is stdout, add it only if flag base64 is active
	//(otherwise already added when password was loaded)
	if (add_salt_if_required(arg, input) == -1) {
		return (-1);
	}

	//If decryption active, validity of PKCS7 padding must be checked
	if ((arg.option & SSL_CYPHER_DECRYPT)
	&& !(arg.option & SSL_CYPHER_NOPAD) && valid_output(input) == -1) {
		return (-1);
	}

	//If flag base64 is inactive or decription active, output simple
	if (!(arg.option & SSL_CYPHER_BASE64) || (arg.option & SSL_CYPHER_DECRYPT)) {
		ret = cypher_output(arg.av_output, *input, 0);
		free_string(input);
		return (ret);
	}

	//Otherwise, encode in base64 before output
	output = encode_base64(input->s, input->len);
	free_string(input);
	if (output.err != 0) {
		return (-1);
	}
	ret = cypher_output(arg.av_output, output, 1);
	free_string(&output);
	return (ret);
}

int			des_input(t_cypher *arg, t_string *input, char *algo)
{
	int	ret;

	if (!(arg->option & SSL_CYPHER_DECRYPT)) {
		ret = encode_io(arg, input, algo);
	}
	else {
		ret = decode_io(arg, input, algo);
	}
	if (ret == -1) {
		return (ret);
	}
	if (arg->option & SSL_CYPHER_INFO) {
		des_print_info(*arg);
		return (0);
	}
	return (1);
}
