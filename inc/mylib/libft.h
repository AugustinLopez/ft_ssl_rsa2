/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 20:56:19 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/03 20:47:41 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

#include <stdint.h>

# define __42__

# ifndef __42__

# include <string.h>
# define ft_memcpy memcpy
# define ft_memset memset
# define ft_memmove memmove
# define ft_memcmp memcmp
# define ft_strlen strlen
# define ft_strcmp strcmp
# define ft_strncmp strncmp
# define ft_strncpy strncpy
# define ft_strchr strchr
# define ft_strncat strncat
# else

#include <stddef.h>
void *ft_memcpy(void *s1, const void *s2, size_t n);
void *ft_memccpy(void *s1, const void *s2, int c, size_t n);
void *ft_memset(void *s, int c, size_t n);
void *ft_memmove(void *s1, const void *s2, size_t n);
int ft_memcmp(const void *s1, const void *s2, size_t n);
size_t ft_strlen(const char *s);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
char *ft_strncpy(char *s1, const char *s2, size_t n);
char *ft_strchr(const char *s, int c);
char *ft_strncat(char *s1, const char *s2, size_t n);

# endif

int fdinput(char *in, char *progname);
int fdoutput(char *out, char *progname);
void print_err(char *progname, char *argument, char *description, int err);
char **ft_strsplit(char const *s, char c);
void ft_freesplit(char **split);
uint64_t ft_rand();
#endif
