/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   adler.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:23:35 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:23:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ADLER_H
# define ADLER_H

static unsigned update_adler32(unsigned adler, const unsigned char* data, unsigned len) {
  unsigned s1 = adler & 0xffff;
  unsigned s2 = (adler >> 16) & 0xffff;

  while(len > 0) {
    
    unsigned amount = len > 5552 ? 5552 : len;
    len -= amount;
    while(amount > 0) {
      s1 += (*data++);
      s2 += s1;
      --amount;
    }
    s1 %= 65521;
    s2 %= 65521;
  }

  return (s2 << 16) | s1;
}


static unsigned adler32(const unsigned char* data, unsigned len) {
  return update_adler32(1L, data, len);
}



#endif
