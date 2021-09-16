/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_numwrite.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/16 16:13:45 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/16 16:31:00 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdint.h>

void ft_numwrite(int fd, uint64_t n)
{
	size_t len;
	size_t save;
	uint64_t m;
	char buf[30];

	len = 1;
	m = n;
	while ((m /= 10))
		++len;
	buf[len] = 0;
	save = len;
	while (len > 0) {
		buf[--len] = '0' + n % 10;
		n /= 10;
	}
	write(fd, buf, save);
}
