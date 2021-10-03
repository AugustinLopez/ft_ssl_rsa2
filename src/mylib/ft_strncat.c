/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/07 15:13:54 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/03 20:46:17 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "mylib/libft.h"

char	*ft_strncat(char *s1, const char *s2, size_t n)
{
	char	*pc;
	size_t	len;

	len = ft_strlen(s1);
	if (!(pc = ft_memccpy(s1 + len, s2, 0, n)))
		*(s1 + len + n) = 0;
	return (s1);
}
