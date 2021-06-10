# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/11/06 13:11:21 by aulopez           #+#    #+#              #
#    Updated: 2021/06/10 19:07:37 by aulopez          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME=ft_ssl
COMPILER=gcc
FLAGS=-g3 -fsanitize=address -Wall -Wextra -MMD -MP

VPATH=	src/cmd:src/mylib:src/digest:src/cypher:src/cypher/base64
PATH_OBJ=.object/
PATH_HDR=./inc/

CC_O=$(COMPILER) $(FLAGS) -c -I$(PATH_HDR)
CC_C=$(COMPILER) $(FLAGS) $(OBJ)

CMD=main type_none type_digest type_cypher
MYLIB=ft_string ft_print ft_strsplit ft_funinfo ft_rand
DIGEST=digest di_utils md5 sha256 sha512
CYPHER=cypher cypher64 decode_b64 encode_b64 des_input des_output \
	des_algorithm des_ecb des3_ecb des_cbc
SRC=$(CMD) $(MYLIB) $(DIGEST) $(CYPHER)
OBJ=$(SRC:%=$(PATH_OBJ)%.o)
DEP=$(OBJ:%.o=%.d)

# --- Rules --------------------------------------------------------------------

all: $(NAME)

$(NAME): $(OBJ)
	-@printf " ===> \e[32mCreated executable\e[0m: $(NAME)\n"
	@$(CC_C) -o $(NAME)

$(PATH_OBJ)%.o:%.c | $(PATH_OBJ)
	-@printf " > \e[33m$*.o\e[0m\n"
	@$(CC_O) $< -o $@

$(PATH_OBJ):
	-@printf " ===> \e[32mCreated directory\e[0m: $(PATH_OBJ)\n"
	@mkdir -p $@

clean:
	-@printf " ===> \e[31mDeleted directory\e[0m: $(PATH_OBJ)\n"
	@rm -rf $(PATH_OBJ)

fclean: clean
	-@printf " ===> \e[31mDeleted executable\e[0m: $(NAME)\n"
	@rm -f $(NAME)

re: fclean $(PATH_OBJ) all

.PHONY: all clean fclean re

-include $(DEP)
