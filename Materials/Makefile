NAME			= ft_irc
CPP				= c++
RMIF			= rm -f
CPPFLAGS		= -Wall -Werror -Wextra -std=c++98 -I.

SERVER_DIR		= ./Server
CLIENT_DIR		= ./Client
COMMAND_DIR		= ./Command
CHANNEL_DIR		= ./Channel
DEBUGGER_DIR	= ./Debugger

RED				= \033[1;31m
GREEN			= \033[1;32m
YELLOW 			= \033[1;33m
DEFAULT			= \033[0m

# List of source files
SRCS	=	$(wildcard $(SERVER_DIR)/*.cpp) \
			$(wildcard $(CLIENT_DIR)/*.cpp) \
			$(wildcard $(COMMAND_DIR)/*.cpp) \
			$(wildcard $(CHANNEL_DIR)/*.cpp) \
			$(wildcard $(DEBUGGER_DIR)/*.cpp) \
			$(wildcard ./*.cpp)

HEADERS =	$(wildcard $(SERVER_DIR)/*.hpp) \
			$(wildcard $(CLIENT_DIR)/*.hpp) \
			$(wildcard $(COMMAND_DIR)/*.hpp) \
			$(wildcard $(CHANNEL_DIR)/*.hpp) \
			$(wildcard $(DEBUGGER_DIR)/*.hpp) \
			$(wildcard ./*.hpp)

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
