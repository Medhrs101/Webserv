# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ymarji <ymarji@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/22 20:14:43 by ymarji            #+#    #+#              #
#    Updated: 2022/02/26 18:55:51 by ymarji           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = clang++
CFLAGS = -Wall -Wextra -Werror --std=c++98

INCLUDES = includes
SRCDIR = sources
OBJDIR = object

HEADERS = ConfigParser.hpp \
	IOhandler.hpp \
	Location.hpp \
	ServerData.hpp \
	Webserv.hpp \
	Webserver.hpp \
	queue.hpp \
	request.hpp \
	socket.hpp 

SRC = ConfigParser.cpp \
	IOhandler.cpp \
	Location.cpp \
	ServerData.cpp \
	WebServer.cpp \
	main.cpp \
	outputLogs.cpp \
	queue.cpp \
	request.cpp \
	socket.cpp

OBJ = $(SRC:.cpp=.o)
OBJ := $(addprefix ./$(OBJDIR)/, $(OBJ))

NAME = Webserv

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

./$(OBJDIR)/%.o: $(SRCDIR)/%.cpp ./$(INCLUDES)/*.hpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f outputParser
	@rm -f webserv_logs
	@rm -f $(NAME)

re: fclean all