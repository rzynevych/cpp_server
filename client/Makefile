NAME = client.exe
SRC =	src/main.cpp

CC = clang++
HEADER_DIR	=	includes/
HEADER		=	includes/client_common.hpp 

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
