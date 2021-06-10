/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 20:56:19 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 18:29:45 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

#include <stdint.h>

# ifndef __42__
# include <string.h>
# define ft_memcpy memcpy
# define ft_memset memset
# define ft_memmove memmove
# define ft_memcmp memcmp
# define ft_strlen strlen
# define ft_strcmp strcmp
# define ft_strncpy strncpy
# define ft_strchr strchr
# define ft_strncat strncat
# endif

void print_err(char *progname, char *argument, char *description, int err);
char **ft_strsplit(char const *s, char c);
void ft_freesplit(char **split);
uint64_t ft_rand();
#endif
