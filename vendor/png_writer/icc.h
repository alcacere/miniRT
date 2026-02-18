/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:37:53 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:37:53 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICC_H
#define ICC_H

#include "str.h"

static inline void lodepng_clear_icc(LodePNGInfo *info)
{
	string_cleanup(&info->iccp_name);
	lodepng_free(info->iccp_profile);
	info->iccp_profile = NULL;
	info->iccp_profile_size = 0;
	info->iccp_defined = 0;
}

static inline unsigned lodepng_assign_icc(LodePNGInfo *info, const char *name, const unsigned char *profile, unsigned profile_size)
{
	info->iccp_name = alloc_string(name);
	info->iccp_profile = (unsigned char *)lodepng_malloc(profile_size);

	if (!info->iccp_name || !info->iccp_profile)
		return 83;

	memcpy(info->iccp_profile, profile, profile_size);
	info->iccp_profile_size = profile_size;

	return 0;
}

static inline unsigned lodepng_set_icc(LodePNGInfo *info, const char *name, const unsigned char *profile, unsigned profile_size)
{
	if (info->iccp_name)
		lodepng_clear_icc(info);
	info->iccp_defined = 1;

	return lodepng_assign_icc(info, name, profile, profile_size);
}

#endif