/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/27 09:46:41 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/libft.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int fdinput(char *in, char *progname) {
	int fdin = STDIN_FILENO;

	if (in != NULL)
		fdin = open(in, O_RDONLY);
	if (fdin < 0)
		print_err(progname, in, 0, errno);
	return (fdin);
}

int fdoutput(char *out, char *progname) {
	int fdout = STDOUT_FILENO;

	if (out != NULL)
		fdout = open(out, O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fdout < 0)
		print_err(progname, out, 0, errno);
	return (fdout);
}
