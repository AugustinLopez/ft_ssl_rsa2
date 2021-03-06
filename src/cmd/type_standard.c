/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_standard.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 01:28:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/16 14:46:52 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "st_cmd.h"

int genrsa_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "genrsa", av[0], TYPE_STANDARD));
	return (cmd_genrsa(ac, av));
}

int rsautl_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "rsautl", av[0], TYPE_STANDARD));
	return (cmd_rsautl(ac, av));
}

int rsa_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "rsa", av[0], TYPE_STANDARD));
	return (cmd_rsa(ac, av));
}
