/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_digest.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 17:39:54 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIGEST_H
# define DIGEST_H

#include <stdint.h>
# define DIGEST_ECHO 0x1
# define DIGEST_QUIET 0x2
# define DIGEST_REVERSE 0x4
# define DIGEST_HELP 0x8

# define DIGEST_NOT_OPT -1
# define DIGEST_INVALID_S -2
# define DIGEST_NULL -3
# define DIGEST_ERRNO -4

# define DIGEST_IS_STDIN 0
# define DIGEST_IS_FILE 1
# define DIGEST_IS_STRING 2

# define DIGEST_HELP_MSG \
	"usage: [digest] [-pqrh] [-s text] [file]\n\n" \
	"-p      \tEcho STDIN to STDOUT and append the checksum to STDOUT\n" \
	"-q      \tPrint the checksum only\n" \
	"-r      \tReverse the output format\n" \
	"-h      \tPrint this help in stdin (instead of stderr)\n" \
	"-s text \tPrint the checksum of the specified text\n\n"

typedef char *(* t_fun_dig)(uint8_t *, size_t);

typedef struct	s_list_arg {
	struct s_list_arg	*next;
	char				*str;
	int					is_file;
}						t_list_arg;

typedef struct	s_digest {
	t_list_arg	*begin;
	t_list_arg	*end;
	uint8_t		option;
	int8_t		err;
}				t_digest;

void		free_digest(t_digest *dig);
t_digest	load_digest_option(int ac, char **av);

char		*md5(uint8_t *tocrypt, size_t len);
char		*sha224(uint8_t *tocrypt, size_t len);
char		*sha256(uint8_t *tocrypt, size_t len);
char		*sha384(uint8_t *tocrypt, size_t len);
char		*sha512(uint8_t *tocrypt, size_t len);
char		*sha512_224(uint8_t *tocrypt, size_t len);
char		*sha512_256(uint8_t *tocrypt, size_t len);
#endif
