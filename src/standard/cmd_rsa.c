/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_rsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/27 12:41:15 by aulopez          ###   ########.fr       */
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

	sfree(arg->sin);
	sfree(arg->sout);
	if (arg->fdin != STDIN_FILENO)
		close(arg->fdin);
	if (arg->fdout != STDOUT_FILENO)
		close(arg->fdout);
	errno = errmem;
	return (ret);
}

static int parsing(int ac, char **av, t_sslrsa *arg) {
	char *fin = NULL;
	char *fout = NULL;

	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDOUT_FILENO, RSA_HELP_MSG, ft_strlen(RSA_HELP_MSG));
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
	arg->fdin = fdinput(fin, "rsa");
	if (arg->fdin == -1)
		return (free_sslrsa(arg, -1));
	arg->fdout = fdoutput(fout, "rsa");
	if (arg->fdout == -1)
		return (free_sslrsa(arg, -1));
	return (1);
}

/*
int add_number(t_string *str, uint8_t *rsa, int size)
{
	char buff[8];
	int i;
	int j;

	size += (rsa[0] >= 0x80);
	ft_memset(buff, 0, sizeof(buff));
	buff[0] = 0x02;
	if (size < 128) {
		buff[1] = (uint8_t)size;
		satrail(str, buff, 2);
	}
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
		satrail(str, buff, 2 + i);
	}
	if (rsa[0] >= 0x80) {
		satrail(str, "\x00", 1);
		size -= 1;
	}
	for (i = 0; i < size; i++)
		satrail(str, (char *)&rsa[i], 1);
	for (i = 0; i < 16; i++)
	return (0);
}

int add_sequence(t_string *str)
{
	char buff[8];
	int i;
	int j;
	uint32_t size;

	salead(str, "\x02\x01\x00", 3);
	size = slen(str);
	ft_memset(buff, 0, sizeof(buff));
	buff[0] = 0x30;
	if (size < 128) {
		buff[1] = (uint8_t)size;
		salead(str, buff, 2);
	}
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
		salead(str, (const char *)buff, 2 + i);
	}
	return (0);
}

int adding_sequence(t_string *str)
{
	char buff[8];
	int i;
	int j;
	uint32_t size;

	salead(str, "\x02\x01\x00", 3);
	size = slen(str);
	ft_memset(buff, 0, sizeof(buff));
	buff[0] = 0x30;
	if (size < 128) {
		buff[1] = (uint8_t)size;
		salead(str, buff, 2);
	}
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
		salead(str, (const char *)buff, 2 + i);
	}
	return (0);
}



int add_pub_sequence(t_string *str)
{
	char buff[10];
	int i;
	int j;
	uint32_t size;

	size = slen(str);
	ft_memset(buff, 0, sizeof(buff));
	buff[0] = 0x30;
	if (size < 128) {
		buff[1] = (uint8_t)size;
		salead(str, buff, 2);
	}
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
		salead(str, (const char *)buff, 2 + i);
	}
	salead(str, "\x00", 1);
	size = slen(str);
	buff[0] = 0x03;
	if (size < 128) {
		buff[1] = (uint8_t)size;
		salead(str, buff, 2);
	}
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
		buff[2 + j] = 0;
		salead(str, (const char *)buff, 2 + i);
	}
	salead(str, "\x30\x0d\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01\x05\x00", 15);
	buff[0] = 0x30;
	size = slen(str);
	if (size < 128) {
		buff[1] = (uint8_t)size;
		salead(str, buff, 2);
	}
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
		salead(str, (const char *)buff, 2 + i);
	}
	return (0);
}



int encode_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	t_string *output;
	t_string *b64;
	int ret;
	int i;
	unsigned char j;

	if (arg->des != 0) {
		rsa->decrypt = 0;
		rsa->key_count = arg->des;
		if (rsa_load_key(rsa, arg->argout) == -1)
			return (-1);
	}
	ret = 0;
	if (arg->pubout == 0) {
		write(arg->fdout, "-----BEGIN RSA PRIVATE KEY-----\n", 32);
		ret = salloc(&output, NULL, 0);
		for (i = 0; i < RSA_MEMBER_COUNT; i++)
			ret = ret != 0 ? -1 : add_number(output, rsa->member[i], rsa->size[i]);
		ret = ret != 0 ? -1 : adding_sequence(output);
		if (arg->des != 0) {
			write(arg->fdout, "Proc-Type: 4,ENCRYPTED\nDEK-Info: DES-", 37);
			if (arg->des == 1)
				write(arg->fdout, "CBC,", 4);
			else
				write(arg->fdout, "EDE3-CBC,", 9);
			for (i = 0; i < 8; i++) {
				if (rsa->salt[i] / 16 >= 10)
					j = (rsa->salt[i] / 16) - 10 + 'A';
				else
					j = (rsa->salt[i] / 16) + '0';
				write(arg->fdout, &j, 1);
				if (rsa->salt[i] % 16 >= 10)
					j = (rsa->salt[i] % 16) - 10 + 'A';
				else
					j = (rsa->salt[i] % 16) + '0';
				write(arg->fdout, &j, 1);
			}
			write(arg->fdout, "\n\n", 2);
			pkcs7_padding(&output);
			if (arg->des == 1)
				des_cbc((uint8_t *)sptr(output), slen(output), rsa->nkey, rsa->salt, 0);
			else
				des3_cbc((uint8_t *)sptr(output), slen(output), rsa->nkey, rsa->salt, 0);
		}
		ret = encode_base64(&b64, sptr(output), slen(output));
		sfree(output);
		output = b64;
		while (slen(output)) {
			if (slen(output) > 64)
				write(arg->fdout, sptr(output), 64);
			else
				write(arg->fdout, sptr(output), slen(output));
			srlead(output, 64);
			write(arg->fdout, "\n", 1);
		}
		sfree(output);
		write(arg->fdout, "-----END RSA PRIVATE KEY-----\n", 30);
	}
	else {
		write(arg->fdout, "-----BEGIN PUBLIC KEY-----\n", 27);
		ret = salloc(&output, NULL, 0);
		ret = ret != 0 ? -1 : add_number(output, rsa->member[0], rsa->size[0]);
		ret = ret != 0 ? -1 : add_number(output, rsa->member[1], rsa->size[1]);
		ret = ret != 0 ? -1 : add_pub_sequence(output);
		ret = encode_base64(&b64, sptr(output), slen(output));
		sfree(output);
		output = b64;
		while (slen(output)) {
			if (slen(output) > 64)
				write(arg->fdout, sptr(output), 64);
			else
				write(arg->fdout, sptr(output), slen(output));
			srlead(output, 64);
			write(arg->fdout, "\n", 1);
		}
		sfree(output);
		write(arg->fdout, "-----END PUBLIC KEY-----\n", 25);

	}
}
*/
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
	if (arg.pubin == 1 && arg.check == 1) {
		print_err(NULL, NULL, "Only private keys can be checked\n", 0);
	}
	if (sfromfd(&(arg.sin), arg.fdin) == -1)
		return (free_sslrsa(&arg, -1));
	if (decode_rsa(&arg, &rsa) == -1) {
		if (arg.pubin == 0)
			print_err(NULL, NULL, "unable to load Private Key", 0);
		else
			print_err(NULL, NULL, "unable to load Public Key", 0);
		return (free_sslrsa(&arg, -1));
	}
	if (arg.text == 1)
		print_text(STDOUT_FILENO, &rsa, arg.pubin);
	if (arg.modulus == 1)
		print_modulus(STDOUT_FILENO, rsa.member[MODULO], rsa.size[MODULO]);
	if (arg.check == 1)
		print_check(arg.fdout, &rsa);
	if (arg.noout == 0)
		write(STDERR_FILENO, "writing RSA key\n", 16);
	if (arg.noout == 0 || arg.fdout != STDOUT_FILENO)
		encode_rsa(&arg, &rsa);
	return (free_sslrsa(&arg, 0));
}
