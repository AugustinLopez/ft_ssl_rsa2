/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 10:08:57 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 15:51:54 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include "mylib/libft.h"
#include "mylib/ft_string.h"

static t_string *err_msg(char *progname, char *argument, char *description, int err)
{
	t_string *str;

	if (salloc(&str, NULL, 0) == -1)
		return (NULL);
	if (progname
		&& (satrail(str, progname, ft_strlen(progname)) == -1
		|| satrail(str, ": ", 2) == -1))
		return (NULL);
	if (argument
		&& (satrail(str, "'", 1) == -1
		|| satrail(str, argument, ft_strlen(argument)) == -1
		|| satrail(str, "': ", 3) == -1))
		return (NULL);
	if (description
		&& (satrail(str, description, ft_strlen(description)) == -1
		|| satrail(str, ": ", 2) == -1))
		return (NULL);
	if (err && satrail(str, strerror(err), ft_strlen(strerror(err))) == -1)
		return (NULL);
	if (err == 0 && slen(str) > 2 && srtrail(str, 2) == -1)
		return (NULL);
	if (satrail(str, "\n", 1) == -1)
		return (NULL);
	return (str);
}

void print_err(char *progname, char *argument, char *description, int err)
{
	t_string *str;
	char *tmp;

	str = err_msg(progname, argument, description, errno);
	if (str) {
		write(2, sptr(str), slen(str));
		sfree(str);
		return ;
	}
	tmp = strerror(err);
	write(2, "Program error: ", 15);
	write(2, tmp, ft_strlen(tmp));
	write(2, "\nLog error: ", 12);
	tmp = strerror(errno);
	write(2, tmp, ft_strlen(tmp));
	write(2, "\n", 1);
}
