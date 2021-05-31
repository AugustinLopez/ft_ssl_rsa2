/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_funinfo.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 14:06:45 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 21:55:41 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_FUNINFO_H
# define FT_FUNINFO_H

#include <stddef.h>
#include <sys/types.h>

# define TYPE_NONE 0x0
# define TYPE_DIGEST 0x1
# define TYPE_CYPHER 0x2
# define TYPE_STANDARD 0x3
# define TYPE_ERROR 0x4

# define REQUEST_RUN 0x0
# define REQUEST_INFO 0x1
# define REQUEST_PRINT 0x2

typedef struct t_funinfo {
	char fun_name[256];
	size_t fun_len;
	int fun_type;
	int request;
	int fd;
} t_funinfo;

void fun_info_set(t_funinfo *info, char *progname, int type);
int fun_name_check(t_funinfo *info, char *progname);
void fun_stack_init(t_funinfo *info);
void fun_request_set(t_funinfo *info, int request);
char *fun_name_get(t_funinfo *info);
int fun_type_get(t_funinfo *info);
void fun_type_set(t_funinfo *info, int type);
int fun_request_get(t_funinfo *info);
ssize_t fun_print(t_funinfo *info);
void fun_fd_set(t_funinfo *info, int fd);
int fun_norun(t_funinfo *info, char *progname, char *av_zero, int type);
#endif
