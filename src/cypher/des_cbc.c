/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_cbc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 19:30:16 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "cy_cmd.h"

void	des_cbc(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt)
{
	uint8_t nr48key[3][16][6];
	uint8_t tmp[8];

	ft_memset(nr48key, 0, sizeof(nr48key));
	ft_memset(tmp, 0, sizeof(tmp));
	des_key(1, nkey, nr48key);
	if (decrypt) {
		for (size_t i = 0; i < len; i+= 8) {
			ft_memcpy(tmp, vector, 8);
			ft_memcpy(vector, str + i, 8);
			initial_permutation(str + i);
			des_decode(str + i, nr48key[0]);
			final_permutation(str + i);
			for (int j = 0; j < 8; j++)
				str[i + j] ^= tmp[j];
		}
	}
	else {
		for (size_t i = 0; i < len; i+= 8) {
			for (int j = 0; j < 8; j++)
				str[i + j] ^= vector[j];
			initial_permutation(str + i);
			des_encode(str + i, nr48key[0]);
			final_permutation(str + i);
			ft_memcpy(vector, str + i, 8);
		}
	}
}

int		des_cbc_cypher(t_cypher *arg)
{
	t_string	*input;
	int ret;

	arg->option |= CYPHER_IV;
	if (arg->option & CYPHER_DECRYPT)
		ret = des_input_todecrypt(arg, &input);
	else
		ret = des_input_tocrypt(arg, &input);
	if (ret == -1)
		return (-1);
	if (arg->option & CYPHER_INFO) {
		des_print_info(arg);
		return (0);
	}
	ret = (arg->option & CYPHER_DECRYPT) ? 1 : 0;
	des_cbc((uint8_t *)sptr(input), slen(input), arg->nkey, arg->vector, ret);
	ret = des_output(arg, input);
	sfree(input);
	return (ret);
}
