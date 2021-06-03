/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_cypher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 21:44:46 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/03 17:29:10 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "ft_cypher.h"

int base64_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "base64", av[0], TYPE_CYPHER));
	return (cypher(ac, av, base64_cypher));
}
