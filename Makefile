# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smalloir <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/29 19:53:46 by smalloir          #+#    #+#              #
#    Updated: 2024/06/07 17:59:06 by sam              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS	= main.cpp Server.cpp

OBJS	= ${SRCS:.cpp=.o}

NAME	= ircserv

CC		= c++

CFLAGS	= -std=c++98 -Wall -Werror -Wextra

RM		= rm -f

%.o : %.cpp
	${CC} -c ${CFLAGS} -o $@ $<

all:	${NAME}

${NAME}:	${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}

clean:
	${RM} ${OBJS}

fclean:	clean
	${RM} ${NAME}

re:	fclean ${NAME}

.PHONY:	all clean fclean re
