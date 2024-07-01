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
#           ----------========== {     VARS     } ==========----------

NAME = ircserv
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

#           ----------========== {     SRCS     } ==========----------

SRC =\
	main.cpp\
	Server.cpp\
	Client.cpp\
	utils.cpp\
	Channel.cpp\
	NumericReply.cpp

#           ----------========== {     OBJS     } ==========----------

OBJ_DIR = obj_dir/
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))
OBJ_BONUS = $(addprefix $(OBJ_DIR), $(SRC_BONUS:.cpp=.o))

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
	
bonus:
	@make --no-print-directory -C ./bot

clean:
	@rm -f $(OBJ)
	@echo -n "\e[1;33m[Cleaned] \e[0;m"
	@make clean --no-print-directory -C ./bot

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(OBJ_DIR)
	@echo -n "\e[1;31m+ [fCleaned] \e[0;m\n"
	@make fclean --no-print-directory -C ./bot

re: fclean all

.PHONY: all clean fclean re bonus
