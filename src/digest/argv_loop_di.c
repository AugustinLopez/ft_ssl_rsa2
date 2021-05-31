/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv_loop_di.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/21 15:52:36 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/07 15:20:11 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ssl.h"
# include "common/ssl_string.h"
# include "digest.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>

static void	print_file(char *crypt, t_ssl algorithm, t_digest argv
					, t_list_dig *elem)
{
	if (crypt == NULL)
		fprintf(stderr, SSL_PROGNAME
		": \"%s\": %s\n", elem->str, strerror(errno));
	else if (argv.option & SSL_DIGEST_QUIET)
		printf("%s\n", crypt);
	else if (argv.option & SSL_DIGEST_REVERSE)
		printf("%s %s\n", crypt, elem->str);
	else
		printf("%s (%s) = %s\n", algorithm.fun_name, elem->str, crypt);
}

static void	loop_file(t_ssl algorithm, t_digest argv, t_list_dig *elem)
{
	t_string	str;
	int			fd;
	char		*crypt;

	fd = open(elem->str, O_RDONLY);
	if (fd < 0)
		fprintf(stderr, SSL_PROGNAME": %s: %s\n", elem->str, strerror(errno));
	else
	{
		str = file_as_string(fd);
		if (close(fd) < 0)
			fprintf(stderr, "Warning: %s: %s\n", elem->str, strerror(errno));
		if (str.err != 0)
		{
			fprintf(stderr, SSL_PROGNAME
			": %s: %s\n", elem->str, strerror(errno));
			return ;
		}
		crypt = ((t_fun_digest)algorithm.fun_ptr)(str.s, str.len);
		free_string(&str);
		print_file(crypt, algorithm, argv, elem);
		free(crypt);
	}
}

static void	loop_string(t_ssl algorithm, t_digest argv, t_list_dig *elem)
{
	t_string	str;
	char		*crypt;

	str = init_string();
	if (str.err == 0)
		str = plus_string(str, elem->str, strlen(elem->str));
	if (str.err != 0)
	{
		fprintf(stderr, SSL_PROGNAME ": %s: %s\n", elem->str, strerror(errno));
		return ;
	}
	crypt = ((t_fun_digest)algorithm.fun_ptr)(str.s, str.len);
	free_string(&str);
	if (crypt == NULL)
		fprintf(stderr, SSL_PROGNAME
		": \"%s\": %s\n", elem->str, strerror(errno));
	else if (argv.option & SSL_DIGEST_QUIET)
		printf("%s\n", crypt);
	else if (argv.option & SSL_DIGEST_REVERSE)
		printf("%s \"%s\"\n", crypt, elem->str);
	else
		printf("%s (\"%s\") = %s\n", algorithm.fun_name, elem->str, crypt);
	free(crypt);
}

static void	loop_stdin(t_ssl algorithm, t_digest argv)
{
	t_string	str;
	char		*crypt;

	str = file_as_string(STDIN_FILENO);
	if (str.err != 0)
	{
		fprintf(stderr, SSL_PROGNAME ": stdin: %s\n", strerror(errno));
		return ;
	}
	else if (argv.option & SSL_DIGEST_ECHO)
		write(STDOUT_FILENO, str.s, str.len);
	crypt = ((t_fun_digest)algorithm.fun_ptr)(str.s, str.len);
	free_string(&str);
	if (crypt == NULL)
		fprintf(stderr, SSL_PROGNAME ": stdin: %s\n", strerror(errno));
	else
		printf("%s\n", crypt);
	free(crypt);
}

int			digest_loop(t_ssl algorithm, t_digest argv)
{
	t_list_dig	*elem;

	if ((argv.option & SSL_DIGEST_ECHO) || argv.begin == NULL)
		loop_stdin(algorithm, argv);
	elem = argv.begin;
	while (elem)
	{
		if (elem->is_file)
			loop_file(algorithm, argv, elem);
		else
			loop_string(algorithm, argv, elem);
		elem = elem->next;
	}
	return (0);
}
