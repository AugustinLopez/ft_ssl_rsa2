/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_rsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/30 16:47:33 by aulopez          ###   ########.fr       */
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

static int free_sslrsa(t_sslrsa *arg, int ret)
{
	int errmem = errno;

	sfree(arg->skey);
	sfree(arg->sout);
	if (arg->fdkey != STDIN_FILENO)
		close(arg->fdkey);
	if (arg->fdout != STDOUT_FILENO)
		close(arg->fdout);
	errno = errmem;
	return (ret);
}

static int parsing(int ac, char **av, t_sslrsa *arg) {
	char *fin = NULL;
	char *fout = NULL;

	arg->fdkey = STDIN_FILENO;
	arg->fdout = STDOUT_FILENO;
	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDERR_FILENO, RSA_HELP_MSG, ft_strlen(RSA_HELP_MSG));
			return (0);
		}
		else if ((ft_strcmp(av[i], "-inform") == 0 || ft_strcmp(av[i], "-outform") == 0)
		&& i < ac - 1 && ft_strcmp(av[++i], "PEM") == 0)
			continue;
		else if (ft_strcmp(av[i], "-in") == 0 && i < ac - 1)
			fin = av[++i];
		else if (ft_strcmp(av[i], "-out") == 0 && i < ac - 1)
			fout = av[++i];
		else if (ft_strcmp(av[i], "-passin") == 0 && i < ac - 1)
			arg->argin = av[++i];
		else if (ft_strcmp(av[i], "-passout") == 0 && i < ac - 1)
			arg->argout = av[++i];
		else if (ft_strcmp(av[i], "-pubin") == 0)
			arg->pubin = 1;
		else if (ft_strcmp(av[i], "-pubout") == 0)
			arg->pubout = 1;
		else if (ft_strcmp(av[i], "-des") == 0)
			arg->des = 1;
		else if (ft_strcmp(av[i], "-des3") == 0)
			arg->des = 3;
		else if (ft_strcmp(av[i], "-text") == 0)
			arg->text = 1;
		else if (ft_strcmp(av[i], "-noout") == 0)
			arg->noout = 1;
		else if (ft_strcmp(av[i], "-modulus") == 0)
			arg->modulus = 1;
		else if (ft_strcmp(av[i], "-check") == 0)
			arg->check = 1;
		else {
			print_err("rsa", av[i], "invalid option", 0);
			return (-1);
		}
	}
	if (arg->pubin == 1 && arg->check == 1) {
		print_err(NULL, NULL, "Only private keys can be checked\n", 0);
		return (-1);
	}
	arg->fdkey = fdinput(fin, "rsa");
	arg->fdout = fdoutput(fout, "rsa");
	if (arg->fdkey == -1 || arg->fdout == -1)
		return (free_sslrsa(arg, -1));
	return (1);
}

int cmd_rsa(int ac, char **av)
{
	int i;
	t_sslrsa arg;
	t_rsa rsa;

	ft_memset(&arg, 0, sizeof(arg));
	ft_memset(&rsa, 0, sizeof(rsa));
	i = parsing(ac, av, &arg);
	if (i != 1)
		return (i);
	if (sfromfd(&(arg.skey), arg.fdkey) == -1)
		return (free_sslrsa(&arg, -1));
	if (decode_rsa(&arg, &rsa) == -1) {
		if (arg.pubin == 0)
			print_err(NULL, NULL, "unable to load Private Key", 0);
		else
			print_err(NULL, NULL, "unable to load Public Key", 0);
		return (free_sslrsa(&arg, -1));
	}
	if (arg.text == 1)
		print_text(STDERR_FILENO, &rsa, arg.pubin);
	if (arg.modulus == 1)
		print_modulus(STDERR_FILENO, rsa.member[MODULO], rsa.size[MODULO]);
	if (arg.check == 1)
		print_check(arg.fdout, &rsa);
	if (arg.noout == 0)
		write(STDERR_FILENO, "writing RSA key\n", 16);
	if (arg.noout == 0 || arg.fdout != STDOUT_FILENO)
		encode_rsa(&arg, &rsa);
	free_sslrsa(&arg, 0);
	return (0);
}
