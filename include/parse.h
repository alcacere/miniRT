#ifndef PARSE_H
# define PARSE_H

# include "minirt.h"

int		parse_file(const char *filename, t_scene *scene);
int		parse_line(char *line, t_scene *scene);

int		parse_color(char *str, t_color *color);
int		parse_vec3(char *str, t_vec3 *vec);

int		is_valid_color(t_color c);
int		is_valid_double(char *str);
int		is_normalized(t_vec3 v);

int		parse_ambient(char **tokens, t_scene *scene);
int		parse_camera(char **tokens, t_scene *scene);
int		parse_light(char **tokens, t_scene *scene);

int		parse_sphere(char **tokens, t_scene *scene);
int		parse_plane(char **tokens, t_scene *scene);
int		parse_cylinder(char **tokens, t_scene *scene);
int 	parse_triangle(char **tokens, t_scene *scene);

#endif