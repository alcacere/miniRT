/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       estimate50.c                                                    */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 21:24:17                                             */
/*  Updated:    2026/01/04 21:24:17                                             */
/*                                                                              */
/* ============================================================================ */

#include "../common.h"
#include <stdlib.h>
#include <string.h>

typedef struct s_sample
{
	real_t x;
	real_t p_x;
} t_sample;

/* Comparator for qsort: compare x values */
static int compare_by_x(const void *a, const void *b)
{
	const t_sample *sa = (const t_sample *)a;
	const t_sample *sb = (const t_sample *)b;

	if (sa->x < sb->x)
		return -1;
	else if (sa->x > sb->x)
		return 1;
	return 0;
}

int main(void)
{
	const unsigned int n = 10000;
	t_sample *samples;
	real_t sum;
	real_t x;
	real_t sin_x;
	real_t p_x;
	real_t half_sum;
	real_t halfway_point;
	real_t accum;
	unsigned int i;

	/* Allocate sample array */
	samples = (t_sample *)malloc(n * sizeof(t_sample));
	if (!samples)
	{
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	sum = 0.0;

	/* Generate samples and compute area under curve */
	for (i = 0; i < n; i++)
	{
		x = random_real_interval(0.0, (real_t)(2.0 * PI));
		sin_x = (real_t)sin((double)x);
		p_x = (real_t)exp((double)(-x / (2.0 * PI))) * sin_x * sin_x;
		sum += p_x;

		samples[i].x = x;
		samples[i].p_x = p_x;
	}

	/* Sort samples by x coordinate */
	qsort(samples, n, sizeof(t_sample), compare_by_x);

	/* Find the sample at which we have half of our area */
	half_sum = sum / 2.0;
	halfway_point = 0.0;
	accum = 0.0;
	for (i = 0; i < n; i++)
	{
		accum += samples[i].p_x;
		if (accum >= half_sum)
		{
			halfway_point = samples[i].x;
			break;
		}
	}

	/* Output results */
	printf("Average = %.12f\n", sum / n);
	printf("Area under curve = %.12f\n", 2.0 * PI * sum / n);
	printf("Halfway = %.12f\n", halfway_point);

	/* Cleanup */
	free(samples);
	return 0;
}
