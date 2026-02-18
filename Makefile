NAME        = miniRT

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -I./include -I./libft/include

LIBFT_DIR   = ./libft
LIBFT       = $(LIBFT_DIR)/libft.a

SRC_DIR     = src
OBJ_DIR     = obj

CORE_DIR    = core
MATH_DIR    = math
PARSER_DIR  = parser
OBJ_MOD_DIR = objects

CORE_SRC    = main_parse_test.c
MATH_SRC    = vec3_basic.c vec3_algebra.c matrix_ops.c
PARSER_SRC  = parse_utils.c parse_scene.c parse_elements.c parse_shapes.c
OBJ_MOD_SRC = scene_builders.c scene_list.c

SRCS        = $(addprefix $(SRC_DIR)/$(CORE_DIR)/, $(CORE_SRC)) \
              $(addprefix $(SRC_DIR)/$(MATH_DIR)/, $(MATH_SRC)) \
              $(addprefix $(SRC_DIR)/$(PARSER_DIR)/, $(PARSER_SRC)) \
              $(addprefix $(SRC_DIR)/$(OBJ_MOD_DIR)/, $(OBJ_MOD_SRC))

OBJS        = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))


all: $(LIBFT) $(NAME)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -lm -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
