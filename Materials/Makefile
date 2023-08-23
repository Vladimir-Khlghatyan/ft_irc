NAME	=	ft_irc
# NAMECLIENT	=	ft_irc_client

CPP		=	c++

CPPFLAGS	=	-Wall -Werror -Wextra -std=c++98 -I.

SRC_DIR = ./Server
SRCClient_DIR = ./Client

# List of source files
SRCS	=	$(wildcard $(SRC_DIR)/*.cpp)
SRCSCLIENT	=	$(wildcard $(SRCClient_DIR)/*.cpp)


HEADERS = $(wildcard *.hpp) $(wildcard *.tpp)

RMIF	=	rm -f

OBJ		=	${SRCS:.cpp=.o}
OBJCLIENT	=	${SRCSCLIENT:.cpp=.o}

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

all:		${NAME}

${NAME}:	${OBJ} ${OBJCLIENT}
	${CPP} ${CPPFLAGS} ${OBJ} ${OBJCLIENT} -o ${NAME}
# ${CPP} ${CPPFLAGS}  -o ${NAMECLIENT}

clean:
		   	${RMIF} ${OBJ} ${OBJCLIENT}

fclean:		clean
			${RMIF} ${NAME} ${NAMECLIENT}

re:			fclean all

.PHONY:		all clean fclean rm
