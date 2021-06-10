/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des3_ofb.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/11 01:13:04 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "cy_cmd.h"

void	des3_ofb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt)
{
	uint8_t nr48key[3][16][6];
	uint8_t tmp[8];

	(void)decrypt;
	ft_memset(nr48key, 0, sizeof(nr48key));
	ft_memcpy(tmp, vector, 8);
	des_key(3, nkey, nr48key);
	ft_memcpy(vector, tmp, 8);
	for (size_t i = 0; i < len; i+= 8) {
		initial_permutation(vector);
		des_encode(vector, nr48key[0]);
		final_permutation(vector);
		initial_permutation(vector);
		des_decode(vector, nr48key[1]);
		final_permutation(vector);
		initial_permutation(vector);
		des_encode(vector, nr48key[2]);
		final_permutation(vector);
		for (int j = 0; j < 8; j++) {
			if (j + i >= len)
				return ;
			str[i + j] ^= vector[j];
		}
	}
}

int		des3_ofb_cypher(t_cypher *arg)
{
	t_string	*input;
	int ret;

	arg->option |= (CYPHER_IV | CYPHER_NOPAD | CYPHER_3KEY);
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
	des3_ofb((uint8_t *)sptr(input), slen(input), arg->nkey, arg->vector, ret);
	ret = des_output(arg, input);
	sfree(input);
	return (ret);
}
