/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_cypher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 21:44:46 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 22:07:15 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "cy_cmd.h"

int base64_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "base64", av[0], TYPE_CYPHER));
	return (cypher(ac, av, base64_cypher));
}

int des_ecb_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-ecb", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des_ecb_cypher));
}

int des3_ecb_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-ede3", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des3_ecb_cypher));
}

int des_cbc_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-cbc", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des_cbc_cypher));
}

int des_cfb_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-cfb", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des_cfb_cypher));
}

int des_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des_cbc_cypher));
}

int des3_cbc_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-ede3-cbc", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des3_cbc_cypher));
}

int des3_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des3", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des3_cbc_cypher));
}

int des3_cfb_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-ede3-cfb", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des3_cfb_cypher));
}

int des_ofb_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-ofb", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des_ofb_cypher));
}

int des3_ofb_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "des-ede3-ofb", av[0], TYPE_CYPHER));
	return (cypher(ac, av, des3_ofb_cypher));
}
