NAME = server.exe
SRC =	src/main.cpp \
		src/Server.cpp


CC = clang++
HEADER_DIR	=	includes/
HEADER		=	includes/server_common.hpp \
				includes/Server.hpp \
				includes/Client.hpp

CFLAGS = -g -std=c++11 -I $(HEADER_DIR)

OBJ = $(patsubst %.cpp,%.o,$(SRC))

%.o: %.cpp $(HEADER)
	$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) -lpthread

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
