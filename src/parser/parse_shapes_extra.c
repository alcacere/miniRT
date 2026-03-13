/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_shapes_extra.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 19:25:43 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 17:39:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"
#include <stdlib.h>

int	parse_triangle(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_triangle	*tr;
	t_color		color;

	if (!tokens[1] || !tokens[2] || !tokens[3] || !tokens[4])
		return (0);
	if (!parse_color(tokens[4], &color))
		return (0);
	obj = create_base_object(OBJ_TRIANGLE, color);
	tr = malloc(sizeof(t_triangle));
	if (!obj || !tr || !parse_vec3(tokens[1], &tr->a))
		return (free(obj), free(tr), 0);
	if (!parse_vec3(tokens[2], &tr->b) || !parse_vec3(tokens[3], &tr->c))
		return (free(obj), free(tr), 0);
	apply_material(obj, tokens[5]);
	obj->shape = tr;
	object_add_back(&scene->objects, obj);
	return (1);
}

int	parse_cone(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_cone		*co;
	t_color		color;

	if (!tokens[1] || !tokens[2] || !tokens[3] || !tokens[4] || !tokens[5])
		return (0);
	if (!parse_color(tokens[5], &color))
		return (0);
	obj = create_base_object(OBJ_CONE, color);
	co = malloc(sizeof(t_cone));
	if (!obj || !co || !parse_vec3(tokens[1], &co->center))
		return (free(obj), free(co), 0);
	if (!parse_vec3(tokens[2], &co->axis))
		return (free(obj), free(co), 0);
	apply_material(obj, tokens[6]);
	co->axis = vec3_normalize(co->axis);
	co->radius = ft_atof(tokens[3]) / 2.0;
	co->height = ft_atof(tokens[4]);
	if (co->radius <= 0.0 || co->height <= 0.0)
		return (free(obj), free(co), 0);
	obj->shape = co;
	object_add_back(&scene->objects, obj);
	return (1);
}
