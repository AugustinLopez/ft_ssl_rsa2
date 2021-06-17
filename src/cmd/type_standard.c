/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_standard.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 01:28:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/17 13:35:16 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"

int genrsa_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "genrsa", av[0], TYPE_STANDARD));
	return (cmd_genrsa(ac, av));
}