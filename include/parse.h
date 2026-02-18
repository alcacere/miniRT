#ifndef PARSE_H
# define PARSE_H

# include "minirt.h"

int		parse_file(const char *filename, t_scene *scene);

int		parse_color(char *str, t_color *color);
int		parse_vec3(char *str, t_vec3 *vec);
double	ft_atof(const char *str);

int		is_valid_color(t_color c);
int		is_normalized(t_vec3 v);

#endif