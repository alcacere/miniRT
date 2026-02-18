/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   padding.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:57:05 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:57:05 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PADDING_H
# define PADDING_H

static void removePaddingBits(unsigned char* out, const unsigned char* in,
                              size_t olinebits, size_t ilinebits, unsigned h) {
  /*
  After filtering there are still padding bits if scanlines have non multiple of 8 bit amounts. They need
  to be removed (except at last scanline of (Adam7-reduced) image) before working with pure image buffers
  for the Adam7 code, the color convert code and the output to the user.
  in and out are allowed to be the same buffer, in may also be higher but still overlapping; in must
  have >= ilinebits*h bits, out must have >= olinebits*h bits, olinebits must be <= ilinebits
  also used to move bits after earlier such operations happened, e.g. in a sequence of reduced images from Adam7
  only useful if (ilinebits - olinebits) is a value in the range 1..7
  */
  unsigned y;
  size_t diff = ilinebits - olinebits;
  size_t ibp = 0, obp = 0; 
  for(y = 0; y < h; ++y) {
    size_t x;
    for(x = 0; x < olinebits; ++x) {
      unsigned char bit = readBitFromReversedStream(&ibp, in);
      setBitOfReversedStream(&obp, out, bit);
    }
    ibp += diff;
  }
}


static void addPaddingBits(unsigned char* out, const unsigned char* in,
                           size_t olinebits, size_t ilinebits, unsigned h) {
  /*The opposite of the removePaddingBits function
  olinebits must be >= ilinebits*/
  unsigned y;
  size_t diff = olinebits - ilinebits;
  size_t obp = 0, ibp = 0; 
  for(y = 0; y != h; ++y) {
    size_t x;
    for(x = 0; x < ilinebits; ++x) {
      unsigned char bit = readBitFromReversedStream(&ibp, in);
      setBitOfReversedStream(&obp, out, bit);
    }
    /*obp += diff; --> no, fill in some value in the padding bits too, to avoid
    "Use of uninitialised value of size ###" warning from valgrind*/
    for(x = 0; x != diff; ++x) setBitOfReversedStream(&obp, out, 0);
  }
}

#endif