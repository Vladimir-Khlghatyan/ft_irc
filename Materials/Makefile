NAME	=	ft_irc

CPP		=	c++

CPPFLAGS	=	-Wall -Werror -Wextra -std=c++98 -I.

SRC_DIR = ./Server
SRCCLIENT_DIR = ./Client
SRCCOMMAND_DIR = ./Command
SRCCANNEL_DIR = ./Channel

# List of source files
SRCS		=	$(wildcard $(SRC_DIR)/*.cpp) \
				$(wildcard ./*.cpp)
SRCSCLIENT	=	$(wildcard $(SRCCLIENT_DIR)/*.cpp)
SRCSCOMMAND	=	$(wildcard $(SRCCOMMAND_DIR)/*.cpp)
SRCSCHANNEL	=	$(wildcard $(SRCCANNEL_DIR)/*.cpp)

HEADERS 	= $(wildcard $(SRC_DIR)*.hpp) \
		  	  $(wildcard $(SRCCLIENT_DIR)*.hpp) \
		  	  $(wildcard $(SRCCOMMAND_DIR)*.hpp) \
		  	  $(wildcard $(SRCCANNEL_DIR)*.hpp) \
		  	  $(wildcard ./*.hpp)

RMIF	=	rm -f

OBJ			=	${SRCS:.cpp=.o}
OBJCLIENT	=	${SRCSCLIENT:.cpp=.o}
OBJCOMMAND	=	${SRCSCOMMAND:.cpp=.o}
OBJCHANNEL	=	${SRCSCHANNEL:.cpp=.o}

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

all:		${NAME}

${NAME}:	${OBJ} ${OBJCLIENT} ${OBJCOMMAND} ${OBJCHANNEL}
	${CPP} ${CPPFLAGS} ${OBJ} ${OBJCLIENT} ${OBJCOMMAND} ${OBJCHANNEL} -o ${NAME}

clean:
		   	${RMIF} ${OBJ} ${OBJCLIENT}  ${OBJCOMMAND} ${OBJCHANNEL}

fclean:		clean
			${RMIF} ${NAME} ${NAMECLIENT} ${OBJCOMMAND} ${OBJCHANNEL}

re:			fclean all

.PHONY:		all clean fclean rm
