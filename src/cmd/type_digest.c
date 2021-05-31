/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_digest.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 21:44:46 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/01 00:02:20 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "di_cmd.h"

int md5_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "md5", av[0], TYPE_DIGEST));
	return (digest(ac, av, md5));
}

int sha224_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "sha224", av[0], TYPE_DIGEST));
	return (digest(ac, av, sha224));
}

int sha256_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "sha256", av[0], TYPE_DIGEST));
	return (digest(ac, av, sha256));
}

int sha512_224_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "sha512/224", av[0], TYPE_DIGEST));
	return (digest(ac, av, sha512_224));
}

int sha512_256_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "sha512/256", av[0], TYPE_DIGEST));
	return (digest(ac, av, sha512_256));
}

int sha384_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "sha384", av[0], TYPE_DIGEST));
	return (digest(ac, av, sha384));
}

int sha512_fun(int ac, char **av, t_funinfo *info)
{
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "sha512", av[0], TYPE_DIGEST));
	return (digest(ac, av, sha512));
}
