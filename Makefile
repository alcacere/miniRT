NAME        = miniRT

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -lm -I./include

SRCS        = src/core/main.c \
              src/math/vec3_basic.c \
              src/math/vec3_algebra.c \
              src/math/matrix_ops.c

OBJS        = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lm -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
