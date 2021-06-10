/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 21:20:26 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 14:37:55 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include "mylib/libft.h"

# define STRING_ERR 0x1
# define STRING_BUF 4096

typedef struct s_string {
	char *intern_ptr; //allocated ptr
	char *ptr; //printable ptr (>= intern_ptr)
	size_t max; //max intern_len possible
	size_t len; //printable len
	size_t intern_len; //len + (ptr - intern_ptr)
} t_string;


void sfree(t_string *str)
{
	if (str) {
		if (str->intern_ptr)
			free(str->intern_ptr);
		free(str);
	}
}

void sdel(t_string **str)
{
	sfree(*str);
	*str = NULL;
}

static int adjust_intern(t_string *str, const size_t add)
{
	int resized = 0;
	size_t tmax = str->max;
	char *tmp;

	//add +1 to guarantee NULL termination
	while (str->intern_len + add + 1 >= tmax) {
		resized = 1;
		if (tmax * 2 < tmax) {
			errno = EMSGSIZE;
			return (-1);
		}
		tmax *= 2;
	}
	if (resized == 0)
		return (0);

	//Instead of reallocating, check if we can simply reset the position
	if (str->len + add + 1< str->max) {
		ft_memmove(str->intern_ptr, str->ptr, str->len);
		str->ptr = str->intern_ptr;
		str->intern_len = str->len;
		return (0);
	}

	tmp = malloc(tmax * sizeof(char));
	if (!tmp)
		return (-1);
	if (str->ptr == str->intern_ptr)
		ft_memcpy(tmp, str->intern_ptr, str->intern_len);
	else {
		//Reset the pos ptr
		ft_memcpy(tmp, str->ptr, str->len);
		str->intern_len = str->len;
	}
	free(str->intern_ptr);
	str->intern_ptr = tmp;
	str->ptr = tmp;
	str->max = tmax;
	return (0);
}

//string add after
int satrail(t_string *str, const char *append, const size_t add)
{
	if (adjust_intern(str, add) == -1) {
		sfree(str);
		return (-1);
	}
	ft_memcpy(str->ptr + str->len, append, add);
	str->intern_len += add;
	str->len += add;
	str->ptr[str->len] = '\0';
	return (0);
}

//string add before
int salead(t_string *str, const char *prepend, const size_t add)
{
	if (str->len == 0)
		return (satrail(str, prepend, add));
	if (add <= (size_t)(str->ptr - str->intern_ptr)) {
		ft_memcpy(str->ptr - add, prepend, add);
		str->len += add;
		str->ptr -= add;
		return (0);
	}
	if (adjust_intern(str, add) == -1) {
		sfree(str);
		return (-1);
	}
	ft_memmove(str->intern_ptr + add, str->ptr, str->len);
	ft_memcpy(str->intern_ptr, prepend, add);
	str->len += add;
	str->intern_len = str->len;
	str->ptr[str->len] = '\0';
	return (0);
}

//string remove before
int srlead(t_string *str, size_t remove) {
	if (remove > str->len)
		remove = str->len;
	str->len -= remove;
	str->ptr += remove;
	if(str->len == 0) {
		str->ptr = str->intern_ptr;
		str->intern_len = 0;
		str->ptr[str->len] = '\0';
	}
	return (0);
}

//string remove after
int srtrail(t_string *str, size_t remove) {
	if (remove > str->len)
		remove = str->len;
	str->len -= remove;
	if(str->len == 0) {
		str->ptr = str->intern_ptr;
		str->intern_len = 0;
	}
	str->ptr[str->len] = '\0';
	return (0);
}

size_t slen(t_string *str)
{
	return (str->len);
}

char *sptr(t_string *str)
{
	return (str->ptr);
}

int salloc(t_string **str, const char *src, const size_t len)
{
	t_string *tmp;
	int ret = 0;

	tmp = malloc(sizeof(*tmp));
	if (tmp == NULL)
		return (-1);
	ft_memset(tmp, 0, sizeof(*tmp));
	tmp->max = STRING_BUF;
	if (src) {
		while (len + 1 >= tmp->max) {
			if (tmp->max * 2 < tmp->max) {
				free(tmp);
				errno = EMSGSIZE;
				return (-1);
			}
			tmp->max *= 2;
		}
	}
	tmp->intern_ptr = malloc(tmp->max * sizeof(uint8_t));
	tmp->ptr = tmp->intern_ptr;
	if (tmp->intern_ptr == NULL) {
		free(tmp);
		return (-1);
	}
	if (src)
		ret = satrail(tmp, src, len);
	if (ret == 0)
		*str = tmp;
	return (0);
}

void sclear(t_string *str)
{
	str->len = 0;
	str->intern_len = 0;
	str->ptr = str->intern_ptr;
	str->ptr[0] = '\0';
}

int sreset(t_string **str)
{
	if ((*str)->max == STRING_BUF) {
		sclear(*str);
		return (0);
	}
	sdel(str);
	if (salloc(str, NULL, 0) == -1)
		return (-1);
	return (0);
}

void strim(t_string *str)
{
	while (str->len) {
		if (str->ptr[0] != ' ')
			break ;
			str->ptr++;
			str->len--;
	}
	while (str->len) {
		if (str->ptr[str->len] != ' ')
			break ;
			str->len--;
	}
	str->ptr[str->len] = '\0';
}

int sfromfd(t_string **str, const int fd)
{
	t_string *tmp;
	char buffer[1024];
	ssize_t len;

	if (salloc(&tmp, NULL, 0) == -1)
		return (-1);
	len = read(fd, buffer, 1024);
	while (len > 0) {
		if (satrail(tmp, buffer, (size_t)len) == -1)
			return (-1);
		len = read(fd, buffer, 1024);
	}
	if (len < 0) {
		sfree(tmp);
		return (-1);
	}
	*str = tmp;
	return (0);
}
