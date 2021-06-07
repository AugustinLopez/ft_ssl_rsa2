/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_rand.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 20:56:31 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/07 20:01:32 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

uint64_t		ft_rand(void)
{
	int			fd;
	ssize_t		len;
	uint64_t	ret;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (0);
	len = read(fd, &ret, sizeof(ret));
	if (close(fd) < 0 || len < (ssize_t)sizeof(ret))
		return (0);
	return (ret);
}
