NAME        = miniRT

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -O3

SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = include
LIBFT_DIR   = libft

CORE_DIR    = $(SRC_DIR)/core
MATH_DIR    = $(SRC_DIR)/math
OBJTS_DIR   = $(SRC_DIR)/objects
PARSE_DIR   = $(SRC_DIR)/parser

CORE_SRC    = graphics.c main.c texture.c \
              memory.c camera_ray.c render.c list_utils.c camera_init.c \
              scatter.c

MATH_SRC    = atof.c vec3_basic.c vec3_adv.c ray_utils.c random.c physics.c aabb.c

OBJTS_SRC   = world.c plane.c sphere.c cylinder.c triangle.c bvh.c cone.c

PARSE_SRC   = parse_shapes.c parser.c parse_utils.c parse_env.c parse_shapes_extra.c

SRCS        = $(addprefix $(CORE_DIR)/, $(CORE_SRC)) \
              $(addprefix $(MATH_DIR)/, $(MATH_SRC)) \
              $(addprefix $(OBJTS_DIR)/, $(OBJTS_SRC)) \
              $(addprefix $(PARSE_DIR)/, $(PARSE_SRC))

OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

LIBFT       = $(LIBFT_DIR)/libft.a

MLX_DIR     = ./minilibx-linux/
MLX_FLAGS   = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
MLX_LIB     = $(MLX_DIR)/libmlx.a


INCLUDES    = -I $(INC_DIR) -I $(LIBFT_DIR)/include -I $(MLX_DIR)

all: $(NAME)

$(NAME): $(LIBFT) $(MLX_LIB) $(OBJS)
	@echo "Assembling $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(MLX_FLAGS) -lpthread -lm -o $(NAME)
	@echo "¡MiniRT Engine compile success!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	@echo "Compiling Libft..."
	@make -C $(LIBFT_DIR)

$(MLX_LIB):
	@echo "Compiling MinilibX..."
	@make -C $(MLX_DIR) 2> /dev/null

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)
	@make -C $(LIBFT_DIR) clean
	@make -C $(MLX_DIR) clean > /dev/null 2>&1 || true

fclean: clean
	@echo "Removing executable..."
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
