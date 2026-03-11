/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_swap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:22:02 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>

/*
** XOR bitwise integer swap — no temporary variable needed.
** WARNING: a and b must not point to the same address.
*/
void	ft_swap(int *a, int *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

/*
** XOR bitwise pointer swap via uintptr_t cast.
** Works for any pointer type — cast callee to (void **).
** WARNING: a and b must not point to the same address.
*/
void	ft_swap_ptr(void **a, void **b)
{
	*(uintptr_t *)a ^= *(uintptr_t *)b;
	*(uintptr_t *)b ^= *(uintptr_t *)a;
	*(uintptr_t *)a ^= *(uintptr_t *)b;
}
