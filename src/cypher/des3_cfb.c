/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des3_cfb.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/11 01:10:57 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "cy_cmd.h"

void	des3_cfb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt)
{
	uint8_t nr48key[3][16][6];
	uint8_t tmp[8];
	uint8_t save[8];
	int n[3];

	n[0] = 0;
	n[1] = 1;
	n[2] = 2;
	ft_memset(nr48key, 0, sizeof(nr48key));
	ft_memcpy(save, vector, 8);
	des_key(3, nkey, nr48key);
	ft_memcpy(vector, save, 8);
	ft_memset(tmp, 0, sizeof(tmp));
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
			if (i + j >= len)
				return ;
			tmp[j] = str[i + j];
			str[i + j] ^= vector[j];
			vector[j] = (decrypt == 1) ? tmp[j] : str[i + j];
		}
	}
}

int		des3_cfb_cypher(t_cypher *arg)
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
	des3_cfb((uint8_t *)sptr(input), slen(input), arg->nkey, arg->vector, ret);
	ret = des_output(arg, input);
	sfree(input);
	return (ret);
}
