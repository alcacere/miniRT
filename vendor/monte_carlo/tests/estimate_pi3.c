/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   estimate_pi3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 17:03:13 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 17:08:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../common.h"

int	main(void)
{
	int	inside_circle = 0;
	int	inside_circle_stratified = 0;
	int	sqrt_n = 1000;
	real_t	x;
	real_t	y;

	for (int i = 0; i < sqrt_n ; i++)
	{
		for (int j = 0; j < sqrt_n; j++)
		{
			x = random_real_interval(-1, 1);
			y = random_real_interval(-1, 1);
			if (x * x + y * y < 1)
				inside_circle++;
			x = 2 * ((i + random_real()) / sqrt_n) - 1;
			y = 2 * ((j + random_real()) / sqrt_n) - 1;
			if (x * x + y * y < 1)
				inside_circle_stratified++;
		}
	}
	printf("Regular estimated of PI = %.12f \n Stratified Estimate of PI = %.12f\n",
		(4.0 * inside_circle) / (sqrt_n * sqrt_n),
		(4.0 * inside_circle_stratified) / (sqrt_n * sqrt_n));
}