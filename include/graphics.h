#ifndef GRAPHICS_H
# define GRAPHICS_H

# include "minirt.h"

# ifndef KEY_ESC
#  define KEY_ESC 65307
# endif

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
}	t_img;

void	init_graphics(t_minirt *rt, t_img *img);
int		key_hook(int keycode, t_minirt *rt);
int		close_window(t_minirt *rt);

#endif