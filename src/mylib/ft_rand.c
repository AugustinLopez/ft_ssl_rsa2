/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_rand.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 20:56:31 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 19:11:40 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stddef.h>

static uint64_t	xor64(uint64_t x)
{
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return (x);
}

/*
** Normally I would use time(NULL) to seed my fallback solution but time is forbidden
*/

uint64_t		ft_rand2(uint64_t seed)
{
	static uint64_t	s0 = 11956449406065;
	static uint64_t	s1 = 35468794565487;
	uint64_t		x;
	uint64_t		y;

	if (seed != 0)
	{
		s0 = xor64(xor64(seed));
		s1 = xor64(xor64(s0));
	}
	x = s0;
	y = s1;
	s0 = y;
	x ^= x << 23;
	s1 = x ^ y ^ (x >> 17) ^ (y >> 26);
	return (s1 + y);
}

uint64_t		ft_rand(void)
{
	int			fd;
	ssize_t		len;
	uint64_t	ret;
	char		*tmp;

	errno = 0;
	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (0);
	len = read(fd, &ret, sizeof(ret));
	if (close(fd) < 0)
		return (0);
	return (ret);
}
