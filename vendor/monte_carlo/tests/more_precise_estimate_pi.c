/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   more_precise_estimate_pi.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 16:57:15 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 17:02:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../common.h"

int main(void)
{
	int inside_circle;
	real_t x;
	real_t y;
	int runs;

	runs = 0;
	inside_circle = 0;
	while (runs < 1000000000)
	{
		runs++;
		x = random_real_interval(-1, 1);
		y = random_real_interval(-1, 1);
		if (x * x + y * y < 1)
			inside_circle++;
		if (runs % 100000 == 0)
			printf("\rEstimate of Pi = %.12f", (4.0 * inside_circle) / runs);
		fflush(stdout);
	}
	printf("\n");
	return (0);
}
