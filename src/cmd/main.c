/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 20:30:15 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/16 14:49:44 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <errno.h>
#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "mylib/ft_funinfo.h"
#include "ft_cmd.h"

const t_fun COMMAND[MAX_CMD_NBR] =
	{quit_fun, exit_fun, help_fun
	, genrsa_fun, rsautl_fun, rsa_fun
	, md5_fun, sha224_fun, sha256_fun
	, sha512_224_fun, sha512_256_fun, sha384_fun, sha512_fun
	, base64_fun, des_fun, des_ecb_fun, des_cbc_fun, des_cfb_fun, des_ofb_fun
	, des3_fun, des3_ecb_fun, des3_cbc_fun, des3_cfb_fun, des3_ofb_fun, NULL};

static int proceed(int ac, char **av)
{
	t_funinfo info;

	if (ac <= 0)
		return (-1);
	fun_stack_init(&info);
	fun_request_set(&info, REQUEST_INFO);
	for (int i = 0; COMMAND[i]; i++) {
		if (COMMAND[i](ac, av, &info) == -1)
			continue;
		fun_request_set(&info, REQUEST_RUN);
		return (COMMAND[i](ac, av, &info));
	}
	fun_request_set(&info, REQUEST_RUN);
	fun_type_set(&info, TYPE_ERROR);
	return (help_fun(ac, av, &info));
}

static int read_stdin(char *progname, size_t proglen, t_string **str)
{
	ssize_t len;
	char buff;

	if (progname)
		write(STDOUT_FILENO, progname, proglen);
	len = read(STDIN_FILENO, &buff, 1);
	if (len == 0)
		return (1);
	if (*str && sreset(str) == -1)
		return (-1);
	if (*str == NULL && salloc(str, NULL, 0) == -1)
		return (-1);
	while (len > 0) {
		if (buff == '\n')
			return (0);
		if (satrail(*str, &buff, 1) == -1)
			return (-1);
		len = read(STDIN_FILENO, &buff, 1);
	}
	if (len < 0)
		return (-1);
	if (progname)
		write(STDOUT_FILENO, "\n", 1);
	return (2);
}

int command_line(void)
{
	int ret;
	int err = 0;
	t_string *str = NULL;
	int ac;
	char **av;

	err = 0;
	while (1) {
		ret = read_stdin("ft_ssl> ", 8, &str);
		if (ret == 0)
			av = ft_strsplit(sptr(str), ' ');
		else if (ret == 1)
			break ;
		if (ret == -1 || !av) {
			print_err("ft_ssl", 0, 0, errno);
			if (err++ >= 10) {
				print_err("ft_ssl", 0, "Too many error. Exiting...", 0);
				return (-1);
			}
			continue ;
		}
		else if (ret == 2)
			continue ;
		err = 0;
		if (av[0] != NULL) {
			ac = 0;
			while ((av)[ac])
				ac++;
			proceed(ac, av);
		}
		ft_freesplit(av);
	}
	if (str)
		sfree(str);
	return (0);
}

int main(int ac, char **av)
{
	if (ac < 2)
		return(command_line());
	return (proceed(ac - 1, av + 1));
}
