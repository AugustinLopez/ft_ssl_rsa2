/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cmd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 13:38:52 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/03 14:54:07 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CMD_H
# define FT_CMD_H

#include "mylib/ft_funinfo.h"
# define MAX_CMD_NBR 32

typedef int (* t_fun)(int, char **, t_funinfo *);
extern const t_fun COMMAND[MAX_CMD_NBR];

int quit_fun(int ac, char **av, t_funinfo *info);
int exit_fun(int ac, char **av, t_funinfo *info);
int help_fun(int ac, char **av, t_funinfo *info);

int md5_fun(int ac, char **av, t_funinfo *info);
int sha224_fun(int ac, char **av, t_funinfo *info);
int sha256_fun(int ac, char **av, t_funinfo *info);
int sha512_224_fun(int ac, char **av, t_funinfo *info);
int sha512_256_fun(int ac, char **av, t_funinfo *info);
int sha384_fun(int ac, char **av, t_funinfo *info);
int sha512_fun(int ac, char **av, t_funinfo *info);

int base64_fun(int ac, char **av, t_funinfo *info);
#endif
