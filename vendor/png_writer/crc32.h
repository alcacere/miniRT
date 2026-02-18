/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   crc32.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:31:11 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:31:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CRC32_H
#define CRC32_H

static unsigned lodepng_crc32(const unsigned char *data, size_t length)
{
	unsigned crc = 0xffffffffu;
	for (size_t i = 0; i < length; ++i)
	{
		crc ^= data[i];
		for (int k = 0; k < 8; ++k)
			crc = (crc >> 1) ^ (0xedb88320u & (-(int)(crc & 1)));
	}
	return crc ^ 0xffffffffu;
}

#endif