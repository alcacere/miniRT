NAME        = miniRT

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -ffast-math -Ofast
DEPFLAGS    = -MMD -MP

# ── Debug / sanitizer flags ───────────────────────────────────────
# -O0 -g3       : no optimisation, full debug symbols
# -fno-omit-frame-pointer : readable stack traces with sanitizers
# NOTE: ASan and TSan are mutually exclusive — separate targets below.
BASE_DBG    = -O0 -g3 -fno-omit-frame-pointer \
              -Wall -Wextra -Werror
ASAN_FLAGS  = $(BASE_DBG) \
              -fsanitize=address,undefined
TSAN_FLAGS  = $(BASE_DBG) \
              -fsanitize=thread

# ── Build output layout ──────────────────────────────────────────
BUILD_DIR   = build
BIN_DIR     = $(BUILD_DIR)/bin
OBJ_DIR     = $(BUILD_DIR)/obj
DEP_DIR     = $(BUILD_DIR)/deps
BINARY      = $(BIN_DIR)/$(NAME)

# debug / tsan get their own isolated obj trees so they don't
# stomp on the release build and vice-versa
DBG_OBJ_DIR  = $(BUILD_DIR)/debug/obj
DBG_DEP_DIR  = $(BUILD_DIR)/debug/deps
DBG_BINARY   = $(BUILD_DIR)/debug/bin/$(NAME)_debug

TSN_OBJ_DIR  = $(BUILD_DIR)/tsan/obj
TSN_DEP_DIR  = $(BUILD_DIR)/tsan/deps
TSN_BINARY   = $(BUILD_DIR)/tsan/bin/$(NAME)_tsan

# ── Source layout ────────────────────────────────────────────────
SRC_DIR     = src
INC_DIR     = include
LIBFT_DIR   = vendor/libft

ACCEL_DIR   = $(SRC_DIR)/accelerators
CORE_DIR    = $(SRC_DIR)/core
GEOM_DIR    = $(SRC_DIR)/geometry
MATH_DIR    = $(SRC_DIR)/math
PARSE_DIR   = $(SRC_DIR)/parser
UTILS_DIR   = $(SRC_DIR)/utils

ACCEL_SRC   = aabb.c bvh.c

CORE_SRC    = graphics.c main.c texture.c \
              memory.c camera_ray.c render.c list_utils.c camera_init.c \
              scatter.c color.c

GEOM_SRC    = cone.c cylinder.c plane.c sphere.c triangle.c

MATH_SRC    = atof.c vec3_basic.c vec3_adv.c ray_utils.c random.c

PARSE_SRC   = parse_shapes.c parser.c parse_utils.c parse_env.c parse_shapes_extra.c

UTILS_SRC   = ft_swap.c physics.c world.c

SRCS        = $(addprefix $(ACCEL_DIR)/, $(ACCEL_SRC)) \
              $(addprefix $(CORE_DIR)/, $(CORE_SRC)) \
              $(addprefix $(GEOM_DIR)/, $(GEOM_SRC)) \
              $(addprefix $(MATH_DIR)/, $(MATH_SRC)) \
              $(addprefix $(PARSE_DIR)/, $(PARSE_SRC)) \
              $(addprefix $(UTILS_DIR)/, $(UTILS_SRC))

# build/obj/core/main.o  build/deps/core/main.d  etc.
OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS        = $(SRCS:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d)

# ── Vendor libs ──────────────────────────────────────────────────
LIBFT       = $(LIBFT_DIR)/libft.a

MLX_DIR     = vendor/minilibx-linux
MLX_FLAGS   = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm
MLX_LIB     = $(MLX_DIR)/libmlx.a

INCLUDES    = -I $(INC_DIR) -I $(LIBFT_DIR)/include -I $(MLX_DIR)

# ─────────────────────────────────────────────────────────────────
MAKEFLAGS  := --no-print-directory

all: update_sync $(BINARY)

update_sync:
	@git submodule update --init --recursive

$(BINARY): $(LIBFT) $(MLX_LIB) $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Assembling $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(MLX_FLAGS) -lpthread -lm -o $(BINARY)
	@echo "¡MiniRT Engine compile success!"

# Compile: .o → build/obj/  .d → build/deps/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP_DIR)/$*.d $(INCLUDES) -c $< -o $@

$(LIBFT):
	@echo "Compiling Libft..."
	@make -C $(LIBFT_DIR)

$(MLX_LIB):
	@echo "Compiling MinilibX..."
	@make -C $(MLX_DIR) 2> /dev/null

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR) $(DEP_DIR) \
	         $(BUILD_DIR)/debug/obj $(BUILD_DIR)/debug/deps \
	         $(BUILD_DIR)/tsan/obj  $(BUILD_DIR)/tsan/deps
	@make -C $(LIBFT_DIR) clean
	@make -C $(MLX_DIR) clean > /dev/null 2>&1 || true

fclean: clean
	@echo "Removing all binaries..."
	@rm -rf $(BUILD_DIR)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

test: all
	@bash assets/tests/run_tests.sh

scenes: all
	@bash assets/scenes/run_all.sh

kill:
	@bash assets/scenes/run_all.sh --kill

# ── Debug target (ASan + UBSan) ───────────────────────────────────
# Catches: heap/stack overflows, use-after-free, null deref,
#          signed overflow, invalid shifts, out-of-bounds, etc.
# Usage: make debug && ./build/debug/bin/miniRT_debug assets/scenes/scene.rt
DBG_OBJS    = $(SRCS:$(SRC_DIR)/%.c=$(DBG_OBJ_DIR)/%.o)
DBG_DEPS    = $(SRCS:$(SRC_DIR)/%.c=$(DBG_DEP_DIR)/%.d)

$(DBG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) $(dir $(DBG_DEP_DIR)/$*.d)
	@$(CC) $(ASAN_FLAGS) $(DEPFLAGS) -MF $(DBG_DEP_DIR)/$*.d $(INCLUDES) -c $< -o $@

$(DBG_BINARY): $(LIBFT) $(MLX_LIB) $(DBG_OBJS)
	@mkdir -p $(dir $(DBG_BINARY))
	@echo "Assembling $(NAME)_debug  [ASan + UBSan]..."
	@$(CC) $(ASAN_FLAGS) $(DBG_OBJS) $(LIBFT) $(MLX_FLAGS) -lpthread -lm -o $(DBG_BINARY)
	@echo "Debug binary → $(DBG_BINARY)"

debug: update_sync $(DBG_BINARY)

# ── TSan target (ThreadSanitizer) ────────────────────────────────
# Catches: data races, lock-order inversions, thread leaks.
# Usage: make tsan && ./build/tsan/bin/miniRT_tsan assets/scenes/scene.rt
TSN_OBJS    = $(SRCS:$(SRC_DIR)/%.c=$(TSN_OBJ_DIR)/%.o)
TSN_DEPS    = $(SRCS:$(SRC_DIR)/%.c=$(TSN_DEP_DIR)/%.d)

$(TSN_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) $(dir $(TSN_DEP_DIR)/$*.d)
	@$(CC) $(TSAN_FLAGS) $(DEPFLAGS) -MF $(TSN_DEP_DIR)/$*.d $(INCLUDES) -c $< -o $@

$(TSN_BINARY): $(LIBFT) $(MLX_LIB) $(TSN_OBJS)
	@mkdir -p $(dir $(TSN_BINARY))
	@echo "Assembling $(NAME)_tsan  [ThreadSanitizer]..."
	@$(CC) $(TSAN_FLAGS) $(TSN_OBJS) $(LIBFT) $(MLX_FLAGS) -lpthread -lm -o $(TSN_BINARY)
	@echo "TSan binary → $(TSN_BINARY)"

tsan: update_sync $(TSN_BINARY)
	@printf '#!/bin/sh\nexec setarch %s -R %s "$$@"\n' "$$(uname -m)" \
		"$$(realpath $(TSN_BINARY))" > $(dir $(TSN_BINARY))miniRT_tsan_run
	@chmod +x $(dir $(TSN_BINARY))miniRT_tsan_run
	@echo ""
	@echo "TSan ready — run with:"
	@echo "  $(dir $(TSN_BINARY))miniRT_tsan_run <scene.rt>"

-include $(DEPS)
-include $(DBG_DEPS)
-include $(TSN_DEPS)

.PHONY: all clean fclean re update_sync test scenes kill debug tsan
