/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_input.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/23 16:12:44 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pwd.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "ft_digest.h"
#include "cy_cmd.h"

static inline int	char_to_hex(char c)
{
	if ('0' <= c && c <= '9')
		return (c - '0');
	else if ('a' <= c && c <= 'f')
		return (c - 'a' + 10);
	else if ('A' <= c && c <= 'F')
		return (c - 'A' + 10);
	else
		return (-1);
}

int		c16_to_hex64(uint8_t *hex, char *src)
{
	uint8_t	tmp[16];
	int		right;
	int		left;
	size_t	len;

	//Up to 16 characters are read. If less than 16 are read, the rest is filled
	//with '0'. Characters must be valid hexadecimal digit.
	len = 0;
	ft_memset(tmp, '0', 16);
	while (*(src + len) != 0 && len < 16) {
		if (ft_strchr("0123456789abcdefABCDEF", *(src + len)) == NULL)
			return (-1);
		tmp[len] = src[len];
		 ++len;
	}
	len = 0;
	while (len < 8) {
		left = char_to_hex(tmp[len * 2]);
		right = char_to_hex(tmp[len * 2 + 1]);
		hex[len++] = left * 16 + right;
	}
	return (0);
}

int		key_provided(t_cypher *arg)
{
	int		len;
	char	tmp[49];

	ft_memset(tmp, '0', 48);
	ft_strncpy(tmp, arg->av_key, 48);
	len = (arg->option & CYPHER_3KEY) ? 3 : 1;
	for (int i = 0; i < len; i++) {
		if (c16_to_hex64(arg->nkey + i * 8, tmp + i * 16) == -1) {
			write(STDERR_FILENO, "non-hex_digit\ninvalid hex key value\n", 36);
			return (-1);
		}
	}
	if (!(arg->option & CYPHER_IV))
		return (0);
	if (arg->av_vector == NULL) {
		write(STDERR_FILENO, "iv undefined\n", 13);
		return (-1);
	}
	if (c16_to_hex64((uint8_t *)arg->vector, arg->av_vector) == -1) {
		write(STDERR_FILENO, "non-hex_digit\ninvalid hex vector value\n", 39);
		return (-1);
	}
	return (0);
}

char		*pbkdf(uint8_t *pwd, size_t len, uint8_t *salt, int key)
{
	char		*tmp;
	char		*tmp2;
	char		*tmp3;
	
	//We need 16 bytes per key + 16 bytes for the IV. MD5 is 32 bytes.
	//1 key: pbkdf is MD5(PWD | SALT);
	ft_memcpy(pwd + len, salt, 8);
	tmp = md5((char *)pwd, len + 8);
	if (tmp == NULL || key < 3 || ft_strlen(tmp) >= 64)
		return (tmp);

	//3 keys: pbkdf is MD5((MD5(PWD | SALT)) | PWD | SALT);
	ft_memmove(pwd + 16, pwd, len + 8);
	ft_memset(pwd, 0, 16);
	if (c16_to_hex64(pwd, tmp) == -1 || c16_to_hex64(pwd + 8, tmp + 16) == -1) {
		free(tmp);
		return (NULL);
	}
	tmp2 = md5((char *)pwd, len + 8 + 16);
	if (tmp2 == NULL) {
		free(tmp);
		return (NULL);
	}
	tmp3 = malloc(65 * sizeof(char));
	if (tmp3) {
		ft_strncpy(tmp3, tmp, 32);
		ft_strncpy(tmp3 + 32, tmp2, 32);
		tmp3[64] = 0;
	}
	free(tmp);
	free(tmp2);
	return (tmp3);
}

int			load_password(char *dest, int decrypt, char *algo)
{
	char	ver[_SC_PASS_MAX + 1];
	char	prompt[128];
	size_t  n;

	ft_memset(ver, 0, _SC_PASS_MAX + 1);
	ft_memset(prompt, 0, 128);
	n = ft_strlen(algo);
	ft_strncat(prompt, "Verifying - enter ", 18);
	ft_strncat(prompt + 18, algo, n);
	ft_strncat(prompt + 18 + n, " decryption password: ", 22);
	prompt[18 + n + 22] = 0;
	if (decrypt != 0) {
		ft_strncpy(dest, getpass(prompt), _SC_PASS_MAX);
		if (dest[0] == '\0')
			return (-1);
		return (0);
	}
	prompt[18 + n + 1] = 'e';
	prompt[18 + n + 2] = 'n';
	ft_strncpy(dest, getpass(prompt + 12), _SC_PASS_MAX);
	if (dest[0] != '\0')
		ft_strncpy(ver, getpass(prompt), _SC_PASS_MAX);
	if (dest[0] == '\0' || ver[0] == '\0')
		return (-1);
	else if (ft_strcmp(dest, ver) != 0) {
		write(STDERR_FILENO, "Verify failure\n", 15);
		return (-1);
	}
	return (0);
}

static int	dpassword_provided(t_cypher *arg, t_string **input)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1]; //+ salt + some place for 3key
	int		n;
	t_string *tmp;
	char *digest;
	size_t len;

	//password
	ft_memset(pwd, 0, _SC_PASS_MAX + 1);
	if (arg->av_pass != NULL)
		ft_strncpy(pwd, arg->av_pass, _SC_PASS_MAX);
	else if (load_password(pwd, arg->option & CYPHER_DECRYPT, arg->name) == -1) {
		write(STDERR_FILENO, "bad password read\n", 18);
		return (-1);
	}

	//read text (needed for salt. Could optimize but too much error handling
	if (arg->option & CYPHER_BASE64) {
		if (cypher_input(&tmp, arg->av_input) == -1)
			return (-1);
		n = decode_base64(input, sptr(tmp), slen(tmp));
		sfree(tmp);
	}
	else
		n = cypher_input(input, arg->av_input);
	if (n == -1)
		return (-1);

	//salt
	len = ft_strlen(pwd);
	if (slen(*input) < 8 || ft_memcmp(sptr(*input), "Salted__", 8) != 0 || slen(*input) < 16) {
		write(STDERR_FILENO, "bad magic number\n", 17);
		sdel(input);
		return (-1);
	}
	ft_memcpy(arg->salt, sptr(*input) + 8, 8);
	ft_memcpy(pwd + len, arg->salt, 8);
	//remove salt from saved input
	if (srlead(*input, 16) == 1)
		return (-1);

	//convert pass and salt.
	n = (arg->option & CYPHER_3KEY) ? 3 : 1;
	digest = pbkdf((uint8_t *)pwd, len, arg->salt, n);
	if (digest == NULL)
		return (-1);

	//Put the result in our key(s) and IV (if not provided/required).
	arg->av_key = digest;
	if (arg->av_vector == NULL)
		arg->av_vector = arg->av_key + n * 16;
	n = key_provided(arg);
	free(digest);
	if (arg->av_vector == arg->av_key + n * 16)
		arg->av_vector = NULL;
	arg->av_key = NULL;
	return (n);
}

static int	epassword_provided(t_cypher *arg)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1]; //+ salt + some place for 3key
	char	*tmp;
	size_t	len;
	int		n;

	//password
	ft_memset(pwd, 0, _SC_PASS_MAX + 1);
	if (arg->av_pass != NULL)
		ft_strncpy(pwd, arg->av_pass, _SC_PASS_MAX);
	else if (load_password(pwd, arg->option & CYPHER_DECRYPT, arg->name) == -1) {
		write(STDERR_FILENO, "bad password read\n", 18);
		return (-1);
	}

	//salt
	len = ft_strlen(pwd);
	if (arg->av_salt == NULL) {
		for (int i = 0; i < 8; i ++)
			arg->salt[i] = (uint8_t)ft_rand();
	}
	else if (c16_to_hex64((uint8_t *)arg->salt, arg->av_salt) == -1) {
		write(STDERR_FILENO, "non-hex digit\ninvalid hex salt value\n", 37);
		return (-1);
	}

	//convert pass and salt.
	n = (arg->option & CYPHER_3KEY) ? 3 : 1;
	tmp = pbkdf((uint8_t *)pwd, len, arg->salt, n);
	if (tmp == NULL)
		return (-1);

	//Put the result in our key(s) and IV (if not provided/required).
	arg->av_key = tmp;
	if (arg->av_vector == NULL)
		arg->av_vector = tmp + n * 16;
	n = key_provided(arg);
	free(tmp);
	arg->av_key = NULL;
	if (arg->av_vector == tmp + n * 16)
		arg->av_vector = NULL;
	return (n);
}

int	pkcs7_padding(t_string **topad)
{
	char	val;

	val  = 8 - slen(*topad) % 8;
	for (int len = val; len > 0; len --) {
		if (satrail(*topad, &val, 1) == -1) {
			sdel(topad);
			return (-1);
		}
	}
	return (0);
}

int		des_input_tocrypt(t_cypher *arg, t_string **input)
{
	if (arg->av_key != NULL && key_provided(arg) == -1)
		return (-1);
	else if (arg->av_key == NULL && epassword_provided(arg) == -1)
		return (-1);
	else if (arg->option & CYPHER_INFO)
		return (0);
	if (arg->av_key == NULL && arg->av_output == NULL && !(arg->option & CYPHER_BASE64)) {
		write(STDOUT_FILENO, "Salted__", 8);
		write(STDOUT_FILENO, arg->salt, 8);
	}
	if (cypher_input(input, arg->av_input) == -1)
		return (-1);
	if (arg->option & CYPHER_NOPAD)
		return (0);
	return (pkcs7_padding(input));
}

int		des_input_todecrypt(t_cypher *arg, t_string **input)
{
	t_string *tmp;
	int n;

	*input = NULL;
	if (arg->av_key != NULL && key_provided(arg) == -1)
		return (-1);
	else if (arg->av_key == NULL && dpassword_provided(arg, input) == -1)
		return (-1);
	if (arg->option & CYPHER_INFO) {
		if (arg->av_key == NULL)
			sfree(*input);
		return (0);
	}
	if (arg->av_key == NULL)
		return (0);
	if (arg->option & CYPHER_BASE64) {
		if (cypher_input(&tmp, arg->av_input) == -1)
				return (-1);
		n = decode_base64(input, sptr(tmp), slen(tmp));
		sfree(tmp);
	}
	else
		n = cypher_input(input, arg->av_input);
	if (n == -1)
		return (-1);
	return (n);
}
