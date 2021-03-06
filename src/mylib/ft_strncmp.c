/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/15 11:44:16 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/03 20:50:15 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdint.h>


int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	if (!n++)
		return (0);
	while (--n && *s1 == *s2 && *s1 && *s2)
	{
		++s1;
		++s2;
	}
	if (!n && (*--s1 == *--s2))
		return (0);
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/*
static inline int	align_address(const unsigned char **c1,
						const unsigned char **c2, size_t *n)
{
	while ((uintptr_t)*c1 & 0x7)
	{
		if (**c1 != **c2 || !**c1)
			return (1);
		if (!*--n)
			return (0);
		++*c1;
		++*c2;
	}
	return (0);
}

static inline void	loopword(const unsigned char **c1,
						const unsigned char **c2, size_t *n)
{
	const uint64_t	*ll1;
	const uint64_t	*ll2;

	ll1 = (const uint64_t *)*c1;
	ll2 = (const uint64_t *)*c2;
	while (*n >= 8)
	{
		if ((*ll1 != *ll2)
				|| (((*ll1 - ONE_EACH_BYTE) & ~*ll1) & REV_EACH_BYTE))
			break ;
		*n -= 8;
		++ll1;
		++ll2;
	}
	*c1 = (const unsigned char *)ll1;
	*c2 = (const unsigned char *)ll2;
}

int					ft_strncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char	*cs1;
	const unsigned char	*cs2;

	cs1 = (const unsigned char *)s1;
	cs2 = (const unsigned char *)s2;
	if (n >= 8 && (((uintptr_t)s1 & 0x7) == ((uintptr_t)s2 & 0x7)))
	{
		if (align_address(&cs1, &cs2, &n))
			return (*cs1 - *cs2);
		loopword(&cs1, &cs2, &n);
	}
	if (!n)
		return (0);
	while (*cs1 == *cs2)
	{
		if (!--n)
			return (0);
		else if (!*cs1)
			break ;
		++cs1;
		++cs2;
	}
	return (*cs1 - *cs2);
}
*/
