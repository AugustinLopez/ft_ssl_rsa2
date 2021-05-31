/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_digest.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 21:44:46 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 21:52:11 by aulopez          ###   ########.fr       */
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
