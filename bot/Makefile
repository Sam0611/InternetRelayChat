# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sbeaucie <sbeaucie@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/17 01:45:34 by sbeaucie          #+#    #+#              #
#    Updated: 2023/10/17 01:45:34 by sbeaucie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#           ----------========== {     VARS     } ==========----------

NAME = bot
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

#           ----------========== {     SRCS     } ==========----------

SRC =\
	main.cpp\
	IrcBot.cpp

#           ----------========== {     OBJS     } ==========----------

OBJ_DIR = obj_dir/
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

#           ----------========== {    RULES    } ==========----------

all: $(NAME)

$(OBJ_DIR)%.o: %.cpp
	@$(CC) $(FLAGS) -c $< -o $@
	@echo -n "\e[1;42m \e[0;m"

$(NAME): $(OBJ_DIR) $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo -n "\e[1;32m [Compiled]\e[0;m\n"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -f $(OBJ)
	@echo -n "\e[1;33m[Cleaned] \e[0;m"

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(OBJ_DIR)
	@echo -n "\e[1;31m+ [fCleaned] \e[0;m\n"

re: fclean all

.PHONY: all clean fclean re