/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cmd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 13:38:52 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 17:53:11 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CMD_H
# define FT_CMD_H

#include "mylib/ft_funinfo.h"
# define MAX_CMD_NBR 32

typedef int (* t_fun)(int, char **, t_funinfo *);
extern const t_fun COMMAND[MAX_CMD_NBR];

int quit_fun(int ac, char **av, t_funinfo *info);
int help_fun(int ac, char **av, t_funinfo *info);
int test_fun(int ac, char **av, t_funinfo *info);

#endif
