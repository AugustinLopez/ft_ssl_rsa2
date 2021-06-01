/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_funinfo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 13:48:02 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/01 01:01:25 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include "mylib/libft.h"
#include "mylib/ft_funinfo.h"

void fun_info_set(t_funinfo *info, char *progname, int type)
{
	size_t len;

	info->fun_type = type;
	len = ft_strlen(progname);
	if (len > 255)
		len = 255;
	ft_memcpy(info->fun_name, progname, len);
	info->fun_name[len] = 0;
	info->fun_len = len;
}

int fun_name_check(t_funinfo *info, char *progname)
{
	if (ft_strcmp(info->fun_name, progname))
		return (-1);
	return (0);
}

void fun_stack_init(t_funinfo *info)
{
	info->request = REQUEST_INFO;
	info->fd = STDOUT_FILENO;
	ft_memset(info->fun_name, 0, 256);
	info->fun_type = TYPE_NONE;
}

void fun_request_set(t_funinfo *info, int request)
{
	info->request = request;
}

char *fun_name_get(t_funinfo *info)
{
	return (info->fun_name);
}

int fun_type_get(t_funinfo *info)
{
	return (info->fun_type);
}

int fun_request_get(t_funinfo *info)
{
	return (info->request);
}

void fun_type_set(t_funinfo *info, int type)
{
	info->fun_type = type;
}

void fun_fd_set(t_funinfo *info, int fd)
{
	info->fd = fd;
}

ssize_t fun_print(t_funinfo *info)
{
	char tmp[257];

	ft_memcpy(tmp, info->fun_name, info->fun_len);
	tmp[info->fun_len] = '\n';
	tmp[info->fun_len + 1] = '\0';
	return(write(info->fd, tmp, info->fun_len + 1));
}

int fun_norun(t_funinfo *info, char *progname, char *av_zero, int type)
{
	if (fun_request_get(info) == REQUEST_PRINT) {
		if (fun_type_get(info) == type) {
			fun_info_set(info, progname, type);
			fun_print(info);
		}
	}
	else if (fun_request_get(info) == REQUEST_INFO) {
		fun_info_set(info, progname, type);
		return (fun_name_check(info, av_zero));
	}
	return (0);
}
