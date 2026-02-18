/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inflate.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:15:46 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:15:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INFLATE_H
# define INFLATE_H

static inline unsigned inflateNoCompression(ucvector* out, const unsigned char* in, size_t* bp, size_t* pos, size_t inlength) {
  size_t p;
  unsigned LEN, NLEN, n, error = 0;

  
  while(((*bp) & 0x7) != 0) ++(*bp);
  p = (*bp) / 8; 

  
  if(p + 4 >= inlength) return 52; 
  LEN = in[p] + 256u * in[p + 1]; p += 2;
  NLEN = in[p] + 256u * in[p + 1]; p += 2;

  
  if(LEN + NLEN != 65535) return 21; 

  if(!ucvector_resize(out, (*pos) + LEN)) return 83; 

  
  if(p + LEN > inlength) return 23; 
  for(n = 0; n < LEN; ++n) out->data[(*pos)++] = in[p++];

  (*bp) = p * 8;

  return error;
}

static inline unsigned lodepng_inflatev(ucvector* out,
                                 const unsigned char* in, size_t insize,
                                 const LodePNGDecompressSettings* settings) {
  
  size_t bp = 0;
  unsigned BFINAL = 0;
  size_t pos = 0; 
  unsigned error = 0;

  (void)settings;

  while(!BFINAL) {
    unsigned BTYPE;
    if(bp + 2 >= insize * 8) return 52; 
    BFINAL = readBitFromStream(&bp, in);
    BTYPE = 1u * readBitFromStream(&bp, in);
    BTYPE += 2u * readBitFromStream(&bp, in);

    if(BTYPE == 3) return 20; 
    else if(BTYPE == 0) error = inflateNoCompression(out, in, &bp, &pos, insize); 
    else error = inflateHuffmanBlock(out, in, &bp, &pos, insize, BTYPE); 

    if(error) return error;
  }

  return error;
}

static inline unsigned lodepng_inflate(unsigned char** out, size_t* outsize,
                         const unsigned char* in, size_t insize,
                         const LodePNGDecompressSettings* settings) {
  unsigned error;
  ucvector v;
  ucvector_init_buffer(&v, *out, *outsize);
  error = lodepng_inflatev(&v, in, insize, settings);
  *out = v.data;
  *outsize = v.size;
  return error;
}

static inline unsigned inflate(unsigned char** out, size_t* outsize,
                        const unsigned char* in, size_t insize,
                        const LodePNGDecompressSettings* settings) {
  if(settings->custom_inflate) {
    return settings->custom_inflate(out, outsize, in, insize, settings);
  } else {
    return lodepng_inflate(out, outsize, in, insize, settings);
  }
}
# endif