/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit_help.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 20:30:15 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 17:55:20 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "mylib/ft_funinfo.h"
#include "mylib/libft.h"
#include "ft_cmd.h"

#include "mylib/ft_string.h"

int test_fun(int ac, char **av, t_funinfo *info)
{
	t_string *test;

	(void)ac;
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "test", av[0], TYPE_NONE));
	if (sfromfd(&test, STDIN_FILENO) != -1)
		write(STDOUT_FILENO, sptr(test), slen(test));
	return (0);
}


int quit_fun(int ac, char **av, t_funinfo *info)
{
	(void)ac;
	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "quit", av[0], TYPE_NONE));
	exit(EXIT_SUCCESS);
}

int help_fun(int ac, char **av, t_funinfo *info)
{
	int fd = STDOUT_FILENO;

	if (fun_request_get(info) != REQUEST_RUN)
		return (fun_norun(info, "help", av[0], TYPE_NONE));
	if (fun_type_get(info) == TYPE_ERROR) {
		fd = STDERR_FILENO;
		write(fd, "ft_ssl: Error: '", 16);
		write(fd, av[0], ft_strlen(av[0]));
		write(fd, "' is an invalid command.\n\n", 26);
	}
	fun_fd_set(info, fd);
	fun_request_set(info, REQUEST_PRINT);
	write(fd, "Standard commands:\n", 19);
	fun_type_set(info, TYPE_STANDARD);
	for (int i = 0; COMMAND[i]; i++)
		COMMAND[i](ac, av, info);
	write(fd, "\nMessage Digest commands:\n", 26);
	fun_type_set(info, TYPE_DIGEST);
	for (int i = 0; COMMAND[i]; i++)
		COMMAND[i](ac, av, info);
	write(fd, "\nCypher commands:\n", 18);
	fun_type_set(info, TYPE_CYPHER);
	for (int i = 0; COMMAND[i]; i++)
		COMMAND[i](ac, av, info);
	write(fd, "\n", 1);
	return (0);
}
