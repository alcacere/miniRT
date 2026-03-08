/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:13:26 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 16:13:28 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXTURE_H
# define TEXTURE_H

# include "structures.h"

t_color	apply_checkerboard(t_vec3 hit_point, t_color base_color);
void	apply_bumpmap(t_vec3 p, t_vec3 *normal);

#endif
