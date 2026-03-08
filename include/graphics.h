/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:12:58 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 16:12:59 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
