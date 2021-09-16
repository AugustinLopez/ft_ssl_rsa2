/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_rsautl.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/16 14:30:46 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

static int fdoutput(char *out) {
	int fdout;

	if (out == NULL)
		fdout = STDOUT_FILENO;
	else {
		fdout = open(out, O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fdout < 0) {
			print_err("rsautl", out, 0, errno);
			return (-1);
		}
	}
	return (fdout);
}

static int fdinput(char *in) {
	int fdin;
	
	if (in == NULL)
		fdin = STDIN_FILENO;
	else {
		fdin = open(in, O_RDONLY);
		if (fdin < 0) {
			print_err("rsautl", in, 0, errno);
			return (-1);
		}
	}
	return (fdin);
}

static int parsing(int ac, char **av, t_utl *arg) {
	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDOUT_FILENO, RSAUTL_HELP_MSG, ft_strlen(RSAUTL_HELP_MSG));
			return (0);
		}
		else if (ft_strcmp(av[i], "-in") == 0 && i < ac - 1)
			arg->fin = av[++i];
		else if (ft_strcmp(av[i], "-out") == 0 && i < ac - 1)
			arg->fout = av[++i];
		else if (ft_strcmp(av[i], "-inkey") == 0 && i < ac - 1)
			arg->fkey = av[++i];
		else if (ft_strcmp(av[i], "-pubin") == 0)
			arg->pubin = 1;
		else if (ft_strcmp(av[i], "-encrypt") == 0)
			arg->decrypt = 0;
		else if (ft_strcmp(av[i], "-decrypt") == 0)
			arg->decrypt = 1;
		else if (ft_strcmp(av[i], "-hexdump") == 0)
			arg->hexdump = 1;
		else {
			print_err("rsautl", av[i], "invalid option", 0);
			return (-1);
		}
	}
	return (1);
}

int cmd_rsautl(int ac, char **av)
{
	int ret;
	t_utl arg;
	t_rsa rsa;

	ft_memset(&arg, 0, sizeof(arg));
	ret = parsing(ac, av, &arg);
	if (ret != 1)
		return (ret);
	if (arg.fkey == NULL) {
		print_err(NULL, NULL,"no keyfile specified", 0);
		if (arg.pubin)
			print_err(NULL, NULL, "Unable to load Public key", 0);
		else
			print_err(NULL, NULL, "Unable to load Private key", 0);
		return (-1);
	}
	arg.fdin =fdinput(arg.fin);
	if (arg.fdin == -1)
		return (-1);
	arg.fdkey =fdinput(arg.fkey);
	if (arg.fdkey == -1)
		return (-1);
	if (sfromfd(&(arg.skey), arg.fdkey) == -1)
		return (-1);
	if (sfromfd(&(arg.sin), arg.fdin) == -1) {
		sdel(&(arg.skey));
		return (-1);
	}
	arg.fdout = fdoutput(arg.fout);
	if (arg.fdout == -1) {
		sdel(&(arg.skey));
		sdel(&(arg.sin));
		return (-1);
	}
	if (decode_64rsa(&arg.skey, arg.pubin, &rsa) == -1) {
		sdel(&(arg.skey));
		sdel(&(arg.sin));
		return (-1);
	}
	printf("%d\n", rsa.size[0] * 8);
	return (0);
}
