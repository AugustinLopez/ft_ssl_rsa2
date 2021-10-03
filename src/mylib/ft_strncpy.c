/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/13 15:02:50 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/03 20:50:36 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "mylib/libft.h"

char	*ft_strncpy(char *s1, const char *s2, size_t n)
{
	char	*pc;
	size_t	len;

	if ((pc = ft_memccpy(s1, s2, 0, n)))
	{
		len = pc - s1;
		ft_memset(pc, 0, n - len);
	}
	return (s1);
}
