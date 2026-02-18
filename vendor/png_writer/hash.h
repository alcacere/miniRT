/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:20:17 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:20:17 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HASH_H
#define HASH_H

#include "types.h"

#ifndef HASH_STRUCT_DEFINED
typedef struct s_hash
{
	int *head;
	unsigned short *chain;
	int *val;
	int *headz;
	unsigned short *chainz;
	unsigned short *zeros;
} Hash;
#endif

static unsigned hash_init(Hash *hash, unsigned windowsize)
{
	unsigned i;
	hash->head = (int *)lodepng_malloc(sizeof(int) * HASH_NUM_VALUES);
	hash->val = (int *)lodepng_malloc(sizeof(int) * windowsize);
	hash->chain = (unsigned short *)lodepng_malloc(sizeof(unsigned short) * windowsize);

	hash->zeros = (unsigned short *)lodepng_malloc(sizeof(unsigned short) * windowsize);
	hash->headz = (int *)lodepng_malloc(sizeof(int) * (MAX_SUPPORTED_DEFLATE_LENGTH + 1));
	hash->chainz = (unsigned short *)lodepng_malloc(sizeof(unsigned short) * windowsize);

	if (!hash->head || !hash->chain || !hash->val || !hash->headz || !hash->chainz || !hash->zeros)
	{
		return 83;
	}

	for (i = 0; i != HASH_NUM_VALUES; ++i)
		hash->head[i] = -1;
	for (i = 0; i != windowsize; ++i)
		hash->val[i] = -1;
	for (i = 0; i != windowsize; ++i)
		hash->chain[i] = i;

	for (i = 0; i <= MAX_SUPPORTED_DEFLATE_LENGTH; ++i)
		hash->headz[i] = -1;
	for (i = 0; i != windowsize; ++i)
		hash->chainz[i] = i;

	return 0;
}

static void hash_cleanup(Hash *hash)
{
	lodepng_free(hash->head);
	lodepng_free(hash->val);
	lodepng_free(hash->chain);

	lodepng_free(hash->zeros);
	lodepng_free(hash->headz);
	lodepng_free(hash->chainz);
}

static unsigned getHash(const unsigned char *data, size_t size, size_t pos)
{
	unsigned result = 0;
	if (pos + 2 < size)
	{
		/*A simple shift and xor hash is used. Since the data of PNGs is dominated
		by zeroes due to the filters, a better hash does not have a significant
		effect on speed in traversing the chain, and causes more time spend on
		calculating the hash.*/
		result ^= (unsigned)(data[pos + 0] << 0u);
		result ^= (unsigned)(data[pos + 1] << 4u);
		result ^= (unsigned)(data[pos + 2] << 8u);
	}
	else
	{
		size_t amount, i;
		if (pos >= size)
			return 0;
		amount = size - pos;
		for (i = 0; i != amount; ++i)
			result ^= (unsigned)(data[pos + i] << (i * 8u));
	}
	return result & HASH_BIT_MASK;
}

static unsigned countZeros(const unsigned char *data, size_t size, size_t pos)
{
	const unsigned char *start = data + pos;
	const unsigned char *end = start + MAX_SUPPORTED_DEFLATE_LENGTH;
	if (end > data + size)
		end = data + size;
	data = start;
	while (data != end && *data == 0)
		++data;

	return (unsigned)(data - start);
}

static void updateHashChain(Hash *hash, size_t wpos, unsigned hashval, unsigned short numzeros)
{
	hash->val[wpos] = (int)hashval;
	if (hash->head[hashval] != -1)
		hash->chain[wpos] = hash->head[hashval];
	hash->head[hashval] = (int)wpos;

	hash->zeros[wpos] = numzeros;
	if (hash->headz[numzeros] != -1)
		hash->chainz[wpos] = hash->headz[numzeros];
	hash->headz[numzeros] = (int)wpos;
}
#endif