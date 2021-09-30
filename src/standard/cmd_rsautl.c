/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_rsautl.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/30 16:43:16 by aulopez          ###   ########.fr       */
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

static int free_rsautl(t_sslrsa *arg, int ret)
{
	int errmem = errno;

	sfree(arg->sin);
	sfree(arg->sout);
	sfree(arg->skey);
	if (arg->fdin != STDIN_FILENO)
		close(arg->fdin);
	if (arg->fdout != STDOUT_FILENO)
		close(arg->fdout);
	if (arg->fdkey != STDIN_FILENO)
		close(arg->fdin);
	errno = errmem;
	return (ret);
}

static int parsing(int ac, char **av, t_sslrsa *arg) {
	char *fin = NULL;
	char *fout = NULL;
	char *fkey = NULL;

	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDOUT_FILENO, RSAUTL_HELP_MSG, ft_strlen(RSAUTL_HELP_MSG));
			return (0);
		}
		else if (ft_strcmp(av[i], "-in") == 0 && i < ac - 1)
			fin = av[++i];
		else if (ft_strcmp(av[i], "-out") == 0 && i < ac - 1)
			fout = av[++i];
		else if (ft_strcmp(av[i], "-inkey") == 0 && i < ac - 1)
			fkey = av[++i];
		else if (ft_strcmp(av[i], "-pubin") == 0)
			arg->pubin = 1;
		else if (ft_strcmp(av[i], "-encrypt") == 0)
			arg->decrypt = 0;
		else if (ft_strcmp(av[i], "-decrypt") == 0)
			arg->decrypt = 1;
		else if (ft_strcmp(av[i], "-hexdump") == 0)
			arg->hexdump = 1;
		else if (ft_strcmp(av[i], "-raw") != 0) {
			print_err("rsautl", av[i], "invalid option", 0);
			return (-1);
		}
	}
	if (fkey == NULL) {
		print_err(NULL, NULL, "no keyfile specified", 0);
		return (-1);
	}
	if (arg->pubin == 1 && arg->decrypt == 1) {
		print_err(NULL, NULL, "cannot use decrypt option with public key.", 0);
		return (-1);
	}
	arg->fdin = fdinput(fin, "rsautl");
	arg->fdout = fdoutput(fout, "rsautl");
	arg->fdkey = fdinput(fkey, "rsautl");
	if (arg->fdin == -1 || arg->fdout == -1 || arg->fdkey == -1)
		return (free_rsautl(arg, -1));
	return (1);
}

uint64_t rsautl_64raw(int decrypt, t_rsa *rsa, uint8_t *input)
{
	uint64_t mod = 0;
	uint64_t msg = 0;
	uint64_t exp = 0;
	uint64_t res;
	int option;

	option = decrypt == 1 ? PRIVATE : PUBLIC;
	for (int i = 0; i < rsa->size[MODULO]; i++) {
		mod = (mod << 8) + rsa->member[MODULO][i];
		msg = (msg << 8) + input[i];
	}
	for (int i = 0; i < rsa->size[option]; i++)
		exp = (exp << 8) + rsa->member[option][i];
	res = power_mod(msg, exp, mod);
	return (res);
}

void rsautl_64raw_dump(int fd, uint64_t result, int size, int hexdump)
{
	uint8_t buff[8];
	uint8_t tmp;
	int i;

	if (hexdump == 0) {
		ft_memset(buff, 0, sizeof(buff));
		for (i = 0; i < size; i++)
			buff[i] = (result >> ((size - 1 - i) * 8)) & 0xff;
		write(fd, buff, size);
		return ;
	}
	write(fd, "0000 - ", 7);
	i = 0;
	for (i = 0; i < size; i++) {
		tmp = (result >> ((size - 1 - i) * 8)) & 0xff;
		buff[0] = tmp / 16;
		buff[1] = tmp % 16;
		buff[0] += buff[0] < 10 ? '0' : 'a' - 10;
		buff[1] += buff[1] < 10 ? '0' : 'a' - 10;
		buff[2] = (i == 7) ? '-' : ' ';
		write(fd, buff, 3);
	}
	for (i = size; i < 16; i++)
		write(fd, "   ", 3);
	write(fd, "  ", 2);
	for (i = 0; i < size; i++) {
		tmp = (result >> ((size - 1 - i) * 8)) & 0xff;
		if (tmp >= 0x20 && tmp <= 0x7e)
			write(fd, &tmp, 1);
		else
			write(fd, ".", 1);
	}
	write(fd, "\n", 1);
}

int cmd_rsautl(int ac, char **av)
{
	uint64_t result;
	t_sslrsa arg;
	t_rsa rsa;
	int i;

	ft_memset(&arg, 0, sizeof(arg));
	ft_memset(&rsa, 0, sizeof(rsa));
	i = parsing(ac, av, &arg);
	if (i != 1)
		return (i);
	if (sfromfd(&(arg.skey), arg.fdkey) == -1)
		return (free_rsautl(&arg, -1));
	if (decode_rsa(&arg, &rsa) == -1) {
		if (arg.pubin == 0)
			print_err(NULL, NULL, "unable to load Private Key", 0);
		else
			print_err(NULL, NULL, "unable to load Public Key", 0);
		return (free_rsautl(&arg, -1));
	}
	else if (rsa.size[MODULO] > 8) {
		print_err(NULL, NULL, "program available for up to 64 bit key", 0);
		return (free_rsautl(&arg, -1));
	}
	if (sfromfd(&(arg.sin), arg.fdin) == -1)
		return (free_rsautl(&arg, -1));
	if (slen(arg.sin) != (size_t)rsa.size[MODULO]) {
		print_err(NULL, NULL, "in raw mode, input must have the same size as the key", 0);
		return (free_rsautl(&arg, -1));
	}
	result = rsautl_64raw(arg.decrypt, &rsa, (uint8_t *)sptr(arg.sin));
	rsautl_64raw_dump(arg.fdout, result, rsa.size[MODULO], arg.hexdump);
	free_rsautl(&arg, 0);
	return (0);
}
