/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha2.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/27 18:58:36 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/15 18:04:22 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHA2_H
# define SHA2_H

# include <stddef.h>
# include <stdint.h>
# include "common/ssl_string.h"

typedef struct s_sha2xx
{
	size_t		chunk;
	uint32_t	h[8];
	uint32_t	t[8];
}				t_sha2xx;

typedef struct s_sha5xx
{
	size_t		chunk;
	uint64_t	h[8];
	uint64_t	t[8];
}				t_sha5xx;

t_string	dig_preprocessing(uint8_t *tocrypt, size_t len, int little_endian);
t_string	dig_lpreprocessing(uint8_t *tocrypt, size_t len, int little_endian);
void		*uint8to64(uint64_t *dest, uint8_t *src, size_t len, int endian);
void		*uint8to32(uint32_t *dest, uint8_t *src, size_t len, int endian);
t_sha2xx	sha2xx_algorithm(uint8_t *s, size_t len, int sha);
t_sha5xx	sha5xx_algorithm(uint8_t *s, t_sha5xx algo);
#endif
