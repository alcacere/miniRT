/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:37:14 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:37:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEXT_H
#define TEXT_H

/* Forward declarations */
static inline unsigned lodepng_add_text(LodePNGInfo *info, const char *key, const char *str);
static inline unsigned lodepng_add_itext(LodePNGInfo *info, const char *key, const char *langtag,
										 const char *transkey, const char *str);

static inline void LodePNGText_init(LodePNGInfo *info)
{
	info->text_num = 0;
	info->text_keys = NULL;
	info->text_strings = NULL;
}

static inline void LodePNGText_cleanup(LodePNGInfo *info)
{
	size_t i;
	for (i = 0; i != info->text_num; ++i)
	{
		string_cleanup(&info->text_keys[i]);
		string_cleanup(&info->text_strings[i]);
	}
	lodepng_free(info->text_keys);
	lodepng_free(info->text_strings);
}

static inline unsigned LodePNGText_copy(LodePNGInfo *dest, const LodePNGInfo *source)
{
	size_t i = 0;
	dest->text_keys = 0;
	dest->text_strings = 0;
	dest->text_num = 0;
	for (i = 0; i != source->text_num; ++i)
	{
		CERROR_TRY_RETURN(lodepng_add_text(dest, source->text_keys[i], source->text_strings[i]));
	}
	return 0;
}

static inline void lodepng_clear_text(LodePNGInfo *info)
{
	LodePNGText_cleanup(info);
}

static inline unsigned lodepng_add_text(LodePNGInfo *info, const char *key, const char *str)
{
	char **new_keys = (char **)(lodepng_realloc(info->text_keys, sizeof(char *) * (info->text_num + 1)));
	char **new_strings = (char **)(lodepng_realloc(info->text_strings, sizeof(char *) * (info->text_num + 1)));
	if (!new_keys || !new_strings)
	{
		lodepng_free(new_keys);
		lodepng_free(new_strings);
		return 83;
	}

	++info->text_num;
	info->text_keys = new_keys;
	info->text_strings = new_strings;

	info->text_keys[info->text_num - 1] = alloc_string(key);
	info->text_strings[info->text_num - 1] = alloc_string(str);

	return 0;
}

static inline void LodePNGIText_init(LodePNGInfo *info)
{
	info->itext_num = 0;
	info->itext_keys = NULL;
	info->itext_langtags = NULL;
	info->itext_transkeys = NULL;
	info->itext_strings = NULL;
}

static inline void LodePNGIText_cleanup(LodePNGInfo *info)
{
	size_t i;
	for (i = 0; i != info->itext_num; ++i)
	{
		string_cleanup(&info->itext_keys[i]);
		string_cleanup(&info->itext_langtags[i]);
		string_cleanup(&info->itext_transkeys[i]);
		string_cleanup(&info->itext_strings[i]);
	}
	lodepng_free(info->itext_keys);
	lodepng_free(info->itext_langtags);
	lodepng_free(info->itext_transkeys);
	lodepng_free(info->itext_strings);
}

static inline unsigned LodePNGIText_copy(LodePNGInfo *dest, const LodePNGInfo *source)
{
	size_t i = 0;
	dest->itext_keys = 0;
	dest->itext_langtags = 0;
	dest->itext_transkeys = 0;
	dest->itext_strings = 0;
	dest->itext_num = 0;
	for (i = 0; i != source->itext_num; ++i)
	{
		CERROR_TRY_RETURN(lodepng_add_itext(dest, source->itext_keys[i], source->itext_langtags[i],
											source->itext_transkeys[i], source->itext_strings[i]));
	}
	return 0;
}

static inline void lodepng_clear_itext(LodePNGInfo *info)
{
	LodePNGIText_cleanup(info);
}

static inline unsigned lodepng_add_itext(LodePNGInfo *info, const char *key, const char *langtag,
										 const char *transkey, const char *str)
{
	char **new_keys = (char **)(lodepng_realloc(info->itext_keys, sizeof(char *) * (info->itext_num + 1)));
	char **new_langtags = (char **)(lodepng_realloc(info->itext_langtags, sizeof(char *) * (info->itext_num + 1)));
	char **new_transkeys = (char **)(lodepng_realloc(info->itext_transkeys, sizeof(char *) * (info->itext_num + 1)));
	char **new_strings = (char **)(lodepng_realloc(info->itext_strings, sizeof(char *) * (info->itext_num + 1)));
	if (!new_keys || !new_langtags || !new_transkeys || !new_strings)
	{
		lodepng_free(new_keys);
		lodepng_free(new_langtags);
		lodepng_free(new_transkeys);
		lodepng_free(new_strings);
		return 83;
	}

	++info->itext_num;
	info->itext_keys = new_keys;
	info->itext_langtags = new_langtags;
	info->itext_transkeys = new_transkeys;
	info->itext_strings = new_strings;

	info->itext_keys[info->itext_num - 1] = alloc_string(key);
	info->itext_langtags[info->itext_num - 1] = alloc_string(langtag);
	info->itext_transkeys[info->itext_num - 1] = alloc_string(transkey);
	info->itext_strings[info->itext_num - 1] = alloc_string(str);

	return 0;
}

#endif