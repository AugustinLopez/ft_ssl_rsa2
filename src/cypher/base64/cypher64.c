/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cypher64.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/22 11:47:35 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 15:00:21 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cy_cmd.h"
#include "mylib/ft_string.h"

int	base64_cypher(t_cypher *arg)
{
	t_string	*input;
	t_string	*output;
	int			ret;

	if (cypher_input(&input, arg->av_input) == -1)
		return (-1);
	if (arg->option & CYPHER_DECRYPT)
		ret = decode_base64(&output, sptr(input), slen(input));
	else
		ret = encode_base64(&output, sptr(input), slen(input));
	sfree(input);
	if (ret == -1)
		return (-1);
	if (arg->option & CYPHER_DECRYPT)
		ret = cypher_output(arg->av_output, output, 0);
	else
		ret = cypher_output(arg->av_output, output, 1);
	sfree(output);
	return (ret);
}
