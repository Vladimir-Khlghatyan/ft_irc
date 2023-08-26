NAME	=	ft_irc

CPP		=	c++

CPPFLAGS	=	-Wall -Werror -Wextra -std=c++98 -I.

SRC_DIR = ./Server
SRCClient_DIR = ./Client
SRCCOMMAND_DIR = ./Command

# List of source files
SRCS	=	$(wildcard $(SRC_DIR)/*.cpp)\
			$(wildcard ./*.cpp)
SRCSCLIENT	=	$(wildcard $(SRCClient_DIR)/*.cpp)
SRCSCOMMAND	=	$(wildcard $(SRCCOMMAND_DIR)/*.cpp)

HEADERS = $(wildcard $(SRCCOMMAND_DIR)*.hpp) $(wildcard $(SRCCOMMAND_DIR)*.tpp) \
			$(wildcard $(SRCClient_DIR)*.hpp) $(wildcard $(SRCClient_DIR)*.tpp) \
				$(wildcard $(SRC_DIR)*.hpp) $(wildcard $(SRC_DIR)*.tpp)

RMIF	=	rm -f

OBJ		=	${SRCS:.cpp=.o}
OBJCLIENT	=	${SRCSCLIENT:.cpp=.o}
OBJCOMMAND	=	${SRCSCOMMAND:.cpp=.o}

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

all:		${NAME}

${NAME}:	${OBJ} ${OBJCLIENT} ${OBJCOMMAND}
	${CPP} ${CPPFLAGS} ${OBJ} ${OBJCLIENT} ${OBJCOMMAND} -o ${NAME}

clean:
		   	${RMIF} ${OBJ} ${OBJCLIENT}  ${OBJCOMMAND}

fclean:		clean
			${RMIF} ${NAME} ${NAMECLIENT} ${OBJCOMMAND}

re:			fclean all

.PHONY:		all clean fclean rm
