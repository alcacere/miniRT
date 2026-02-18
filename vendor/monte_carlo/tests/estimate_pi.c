/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   estimate_pi.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:52:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 16:55:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../common.h"

int	main(void)
{
	int	inside_circle;
	int	n;
	real_t x;
	real_t y;

	inside_circle = 0;
	n = 100000;
	for (int i = 0; i < n; i++)
	{
		x = random_real_interval(-1, 1);
		y = random_real_interval(-1, 1);
		if (x * x + y * y < 1)
			inside_circle++;
	}
	printf("Estimate of PI = %.12f\n", 4.0 * inside_circle / n);
}
