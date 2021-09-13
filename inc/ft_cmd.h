/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cmd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 13:38:52 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/13 13:54:13 by aulopez          ###   ########.fr       */
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
int des_ecb_fun(int ac, char **av, t_funinfo *info);
int des3_ecb_fun(int ac, char **av, t_funinfo *info);
int des_cbc_fun(int ac, char **av, t_funinfo *info);
int des_cfb_fun(int ac, char **av, t_funinfo *info);
int des_ofb_fun(int ac, char **av, t_funinfo *info);
int des_fun(int ac, char **av, t_funinfo *info);
int des3_cbc_fun(int ac, char **av, t_funinfo *info);
int des3_fun(int ac, char **av, t_funinfo *info);
int des3_cfb_fun(int ac, char **av, t_funinfo *info);
int des3_ofb_fun(int ac, char **av, t_funinfo *info);

int genrsa_fun(int ac, char **av, t_funinfo *info);
int rsautl_fun(int ac, char **av, t_funinfo *info);
#endif
