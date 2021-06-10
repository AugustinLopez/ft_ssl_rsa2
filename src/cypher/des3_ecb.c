/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des3_ecb.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 18:38:53 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "cy_cmd.h"

void	des3_ecb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt)
{
	uint8_t nr48key[3][16][6];

	(void)vector;
	ft_memset(nr48key, 0, sizeof(nr48key));
	des_key(3, nkey, nr48key);
	if (decrypt) {
		for (size_t i = 0; i < len; i+= 8) {
			initial_permutation(str + i);
			des_decode(str + i, nr48key[2]);
			final_permutation(str + i);
		}
		for (size_t i = 0; i < len; i+= 8) {
			initial_permutation(str + i);
			des_encode(str + i, nr48key[1]);
			final_permutation(str + i);
		}
		for (size_t i = 0; i < len; i+= 8) {
			initial_permutation(str + i);
			des_decode(str + i, nr48key[0]);
			final_permutation(str + i);
		}
		return ;
	}
	for (size_t i = 0; i < len; i+= 8) {
		initial_permutation(str + i);
		des_encode(str + i, nr48key[0]);
		final_permutation(str + i);
	}
	for (size_t i = 0; i < len; i+= 8) {
		initial_permutation(str + i);
		des_decode(str + i, nr48key[1]);
		final_permutation(str + i);
	}
	for (size_t i = 0; i < len; i+= 8) {
		initial_permutation(str + i);
		des_encode(str + i, nr48key[2]);
		final_permutation(str + i);
	}
}

int		des3_ecb_cypher(t_cypher *arg)
{
	t_string	*input;
	int ret;

	arg->option |= CYPHER_3KEY;
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
	des3_ecb((uint8_t *)sptr(input), slen(input), arg->nkey, NULL, ret);
	ret = des_output(arg, input);
	sfree(input);
	return (ret);
}
