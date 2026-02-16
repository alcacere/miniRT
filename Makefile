NAME        = miniRT

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -lm -I./include -I./libft/include
LIBFT		= ./libft/libft.a

SRCS        = src/core/main.c \
              src/math/vec3_basic.c \
              src/math/vec3_algebra.c \
              src/math/matrix_ops.c

OBJS        = $(SRCS:.c=.o)

all: $(NAME) $(LIBFT)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -lm -o $(NAME)

$(LIBFT) :
	@make ./libft

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
