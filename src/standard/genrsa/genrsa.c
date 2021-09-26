/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   genrsa.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 01:28:03 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/13 16:38:55 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"
#include <errno.h>
#include <stdint.h>

uint64_t modmulinv(uint64_t a, uint64_t b)
{
	int64_t t[3] = {0, 1, 0};
	uint64_t r[3] = {b, a % b, 0};
	uint64_t q;

	while (r[1]) {
		q = r[0] / r[1];
		t[2] = t[1];
		t[1] = t[0] - (int64_t)q * t[1];
		t[0] = t[2];
		r[2] = r[1];
		r[1] = r[0] - q * r[1];
		r[0] = r[2];
	}
	if (r[0] > 1)
		return (0);
	if (t[0] < 0)
		t[0] += b;
	return ((uint64_t)t[0]);
}

int addnumber(t_string *str, uint64_t n)
{
	char len;
	char content;

	if (satrail(str, "\x02", 1) == -1)
		return (-1);
	len = 0;
	for (int i = 0; i < 64; i += 8) {
		if ((n >> i) != 0)
			len++;
		else
			break ;
	}
	if (len == 0)
		len = 1;
	content = (n >> ((len - 1) * 8)) & 0xff;
	if ((content & 0x80)) {
		content = len + 1;
		if (satrail(str, &content, 1) == -1 || satrail(str, "\x00", 1) == -1)
			return (-1);
	}
	else if (satrail(str, &len, 1) == -1)
		return (-1);
	for (int i = 64 - len * 8; i < 64; i += 8) {
		content = (n >> (56 - i)) & 0xff;
		if (satrail(str, &content, 1) == -1)
			return (-1);
	}
	return (0);
}

t_string *genrsa(uint64_t p, uint64_t q, uint64_t e)
{
	t_string *str;
	t_string *out;
	uint64_t d;
	int ret;

	if (salloc(&str, "\x30\x00\x02\x01\x00", 5) == -1)
		return (NULL);
	ret = 0;
	ret = ret != 0 ? -1 : addnumber(str, p * q);
	ret = ret != 0 ? -1 : addnumber(str, e);
	d = modmulinv(e, (p - 1) * (q - 1));
	ret = ret != 0 ? -1 : addnumber(str, d);
	ret = ret != 0 ? -1 : addnumber(str, p);
	ret = ret != 0 ? -1 : addnumber(str, q);
	ret = ret != 0 ? -1 : addnumber(str, d % (p - 1));
	ret = ret != 0 ? -1 : addnumber(str, d % (q - 1));
	ret = ret != 0 ? -1 : addnumber(str, modmulinv(q, p));
	if (ret != 0) {
		print_err("genrsa", NULL, NULL, errno);
		return (NULL);
	}
	sptr(str)[1] = slen(str) - 2;
	ret = encode_base64(&out, sptr(str), slen(str));
	sfree(str);
	if (ret == -1) {
		print_err("genrsa", NULL, NULL, errno);
		return (NULL);
	}
	return (out);
}
