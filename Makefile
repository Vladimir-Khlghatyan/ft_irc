NAME			= ircserv
CPP				= c++
RMIF			= rm -f
CPPFLAGS		= -Wall -Werror -Wextra -std=c++98 -I./includes

SRCS_DIR		= ./srcs

HEAD_DIR		= ./includes

RED				= \033[1;31m
GREEN			= \033[1;32m
YELLOW 			= \033[1;33m
DEFAULT			= \033[0m

# List of source files
SRCS	=	$(wildcard $(SRCS_DIR)/*.cpp)

HEADERS =	$(wildcard $(HEAD_DIR)/*.hpp)

OBJS	=	${SRCS:.cpp=.o}

%.o: %.cpp $(HEADERS)
	@$(CPP) $(CPPFLAGS) -c $< -o $@

${NAME}:	${OBJS}
			@${CPP} ${CPPFLAGS} ${OBJS} -o ${NAME}
			@echo "$(GREEN) ✔ $(NAME) created!$(DEFAULT)"

all:		${NAME}

clean:
		   	@${RMIF} ${OBJS}
			@echo "$(YELLOW) ✔ object files deleted!$(DEFAULT)"

fclean:		clean
			@${RMIF} ${NAME}
			@echo "$(RED) ✔ executable deleted!$(DEFAULT)"

re:			fclean all

.PHONY:		all clean fclean rm
