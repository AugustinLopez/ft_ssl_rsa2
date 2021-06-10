/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_output.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 22:49:37 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "cy_cmd.h"
#include <unistd.h>

static int	add_salt_if_required(t_cypher *arg, t_string *input)
{
	char	buf[16];

	if (arg->option & CYPHER_DECRYPT || arg->av_key != NULL)
		return (0);
	if (arg->av_output == NULL && !(arg->option & CYPHER_BASE64))
		return (0);
	ft_memcpy(buf, "Salted__", 8);
	ft_memcpy(buf + 8, arg->salt, 8);
	//salt is added to the left
	if (salead(input, buf, 16) == -1)
		return (-1);
	return (0);
}

static int	valid_output(t_string *input)
{
	uint8_t	ret;

	if (slen(input) == 0)
		ret = 0;
	else
		ret = sptr(input)[slen(input) - 1];
	if (ret < 1 || ret > 16 || slen(input) < ret) {
		write(STDERR_FILENO, "bad magic number\n", 17);
		return (-1);
	}
	for (int i = 0; i < ret;i++) {
		if (sptr(input)[slen(input) - 1 - i] != ret) {
			write(STDERR_FILENO, "bad magic number\n", 17);
			return (-1);
		}
	}
	//padding must be removed from the right before output the decrypted text
	srtrail(input, ret);
	return (0);
}

int			des_output(t_cypher *arg, t_string *input)
{
	t_string	*output;
	int			ret;

	//salt must be added when output is encoded with a password.
	//If output is stdout, add it only if flag base64 is active
	//(otherwise already added when password was loaded)
	if (add_salt_if_required(arg, input) == -1)
		return (-1);

	//If decryption active, validity of PKCS7 padding must be checked
	if ((arg->option & CYPHER_DECRYPT)
	&& !(arg->option & CYPHER_NOPAD) && valid_output(input) == -1)
		return (-1);

	//If flag base64 is inactive or decription active, output simple
	if (!(arg->option & CYPHER_BASE64) || (arg->option & CYPHER_DECRYPT))
		return (cypher_output(arg->av_output, input, 0));

	//Otherwise, encode in base64 before output
	if (encode_base64(&output, sptr(input), slen(input)) == -1)
		return (-1);
	ret = cypher_output(arg->av_output, output, 1);
	sfree(output);
	return (ret);
}

void	des_print_info(t_cypher *arg)
{
	int	n;
	uint8_t *tab[5];
	char buffer[16];
	uint8_t c;

	ft_memset(tab, 0, 5 * sizeof(*tab));
	n = (arg->option & CYPHER_3KEY) ? 3 : 1;
	for (int i = 0 ; i < n; i++)
		tab[i] = arg->nkey + i * 8;
	if (arg->option & CYPHER_IV)
		tab[3] = arg->vector;
	if (arg->av_key == NULL)
		tab[4] = arg->salt;
	for (int i = 0; i < 5; i++) {
		if (tab[i] == 0)
			continue ;
		else if (i == 0)
			write(STDOUT_FILENO, "key=", 4);
		else if (i == 3)
			write(STDOUT_FILENO, "\niv =", 5);
		else if (i == 4)
			write(STDOUT_FILENO, "\nsalt=", 6);
		ft_memset(buffer, 0, 16);
		for (int j = 0; j < 8; j++) {
			c = (tab[i][j] >> 4);
			if (c < 10)
				buffer[j * 2] = '0' + c;
			else
				buffer[j * 2] = 'A' + c - 10;
			c = (tab[i][j] & 0xf);
			if (c < 10)
				buffer[j * 2 + 1] = '0' + c;
			else
				buffer[j * 2 + 1] = 'A' + c - 10;
		}
		write(STDOUT_FILENO, buffer, 16);
	}
	write(STDOUT_FILENO, "\n", 1);
}
