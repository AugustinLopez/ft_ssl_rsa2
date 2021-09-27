/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printrsa.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/27 12:29:00 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"
#include <unistd.h>

static int add_size(t_string *str, uint8_t byte, int size, int trail)
{
	char buff[255];
	int i = 0;

	ft_memset(buff, 0, sizeof(buff));
	buff[0] = byte;
	if (size < 128)
		buff[1] = size % 128;
	else {
		i = 4;
		while ((size >> ((i - 1) * 8)) == 0)
			i--;
		buff[1] = 0x80 + i;
		for (int j = 0; j < i; j++)
			buff[2 + j] = (size >> ((i - j - 1) * 8)) & 0xff;
	}
	if (trail)
		return (satrail(str, buff, 2 + i));
	else
		return (salead(str, buff, 2 + i));
}

static int add_number(t_string *str, uint8_t *rsa, int size)
{
	size += (rsa[0] >= 0x80);
	if (add_size(str, '\x02', size, 1) == -1)
		return (-1);
	if (rsa[0] >= 0x80) {
		if (satrail(str, "\x00", 1) == -1)
			return (-1);
		size -= 1;
	}
	for (int i = 0; i < size; i++)
		if (satrail(str, (char *)&rsa[i], 1) == -1)
			return (-1);
	return (0);
}


static int add_lead_sequence(t_string *str, int pubout)
{
	if (pubout) {
		if (add_size(str, '\x30', slen(str), 0) == -1)
			return (-1);
		if (salead(str, "\x00", 1) == -1)
			return (-1);
		if (add_size(str, '\x03', slen(str), 0) == -1)
			return (-1);
		if (salead(str, "\x30\x0d\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01\x05\x00", 15) == -1)
			return (-1);
		if (add_size(str, '\x30', slen(str), 0) == -1)
			return (-1);
	}
	else {
		if (salead(str, "\x02\x01\x00", 3) == -1)
			return (-1);
		if (add_size(str, '\x30', slen(str), 0) == -1)
			return (-1);
	}
	return (0);
}

static int create_private_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	t_string *output;
	t_string *tmp;
	int ret;
	char buff[19];

	if (arg->des != 0) {
		rsa->decrypt = 0;
		rsa->key_count = arg->des;
		if (rsa_load_key(rsa, arg->argout, arg->argin) == -1)
			return (-1);
		for (int i = 0; i < 8; i++) {
			buff[i * 2] = rsa->salt[i] / 16;
			buff[i * 2 + 1] = rsa->salt[i] % 16;
			buff[i * 2] += buff[i * 2] >= 10 ? 'A' - 10 : '0';
			buff[i * 2 + 1] += buff[i * 2 + 1] >= 10 ? 'A' - 10 : '0';
		}
		ft_memset(buff + 16, '\n', 2);
	}
	ret = salloc(&tmp, NULL, 0);
	for (int i = 0; i < RSA_MEMBER_COUNT; i++)
		ret = ret != 0 ? -1 : add_number(tmp, rsa->member[i], rsa->size[i]);
	ret = ret != 0 ? -1 : add_lead_sequence(tmp, 0);
	if (arg->des != 0) {
		ret = ret != 0 ? -1 : pkcs7_padding(&tmp);
		if (ret == 0 && arg->des == 3)
			des3_cbc((uint8_t *)sptr(tmp), slen(tmp), rsa->nkey, rsa->salt, 0);
		else if (ret == 0)
			des3_cbc((uint8_t *)sptr(tmp), slen(tmp), rsa->nkey, rsa->salt, 0);
	}
	if (ret == -1)
		return (-1);
	ret = encode_base64(&output, sptr(tmp), slen(tmp));
	sfree(tmp);
	if (ret == -1)
		return (-1);
	write(arg->fdout, "-----BEGIN RSA PRIVATE KEY-----\n", 32);
	if (arg->des != 0) {
		write(arg->fdout, "Proc-Type: 4,ENCRYPTED\nDEK-Info: DES-", 37);
		if (arg->des == 1)
			write(arg->fdout, "CBC,", 4);
		else
			write(arg->fdout, "EDE3-CBC,", 9);
		write(arg->fdout, buff, 18);
	}
	while (slen(output)) {
		if (slen(output) > 64)
			write(arg->fdout, sptr(output), 64);
		else
			write(arg->fdout, sptr(output), slen(output));
		srlead(output, 64);
		write(arg->fdout, "\n", 1);
	}
	write(arg->fdout, "-----END RSA PRIVATE KEY-----\n", 30);
	sfree(output);
	return (0);
}

static int create_public_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	t_string *output;
	t_string *tmp;
	int ret;
	
	write(arg->fdout, "-----BEGIN PUBLIC KEY-----\n", 27);
	ret = salloc(&tmp, NULL, 0);
	ret = ret != 0 ? -1 : add_number(tmp, rsa->member[0], rsa->size[0]);
	ret = ret != 0 ? -1 : add_number(tmp, rsa->member[1], rsa->size[1]);
	ret = ret != 0 ? -1 : add_lead_sequence(tmp, 1);
	if (ret == -1)
		return (-1);
	ret = encode_base64(&output, sptr(tmp), slen(tmp));
	sfree(tmp);
	if (ret == -1)
		return (-1);
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
	return (0);
}

int encode_rsa(t_sslrsa *arg, t_rsa *rsa)
{
	if (arg->pubout == 0)
		return (create_private_rsa(arg, rsa));
	return (create_public_rsa(arg, rsa));
}
