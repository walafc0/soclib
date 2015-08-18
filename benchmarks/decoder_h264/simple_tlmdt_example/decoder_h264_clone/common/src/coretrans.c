/*****************************************************************************
  coretrans.c -- File managing the block transformation

  Original Author: 
  Martin Fiedler, CHEMNITZ UNIVERSITY OF TECHNOLOGY, 2004-06-01	
  
  Thales Authors:
  Florian Broekaert, THALES COMMUNICATIONS - AAL, 2006
  Fabien Colas-Bigey, THALES COMMUNICATIONS - AAL, 2008

  Copyright (C) THALES & Martin Fiedler All rights reserved.
 
  This code is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.
   
  This code is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.
   
  You should have received a copy of the GNU General Public License along
  with this code (see file COPYING).  If not, see
  <http://www.gnu.org/licenses/>.
  
  This License does not grant permission to use the name of the copyright
  owner, except only as required above for reproducing the content of
  the copyright notice.
*****************************************************************************/

/****************************************************************************
  Include section
****************************************************************************/
#include "coretrans.h"


/****************************************************************************
  Variables and structures
****************************************************************************/
#define QP0   13107,8066,13107,8066, 8066,5243,8066,5243
#define QP1   11916,7490,11916,7490, 7490,4660,7490,4660
#define QP2   10082,6554,10082,6554, 6554,4194,6554,4194
#define QP3    9362,5825, 9352,5825, 5825,3647,5825,3647
#define QP4    8192,5243, 8192,5243, 5243,3355,5243,3355
#define QP5    7282,4559, 7282,4559, 4559,2893,4559,2893

#define DQP0  10,13,10,13, 13,16,13,16
#define DQP1  11,14,11,14, 14,18,14,18
#define DQP2  13,16,13,16, 16,20,16,20
#define DQP3  14,18,14,18, 18,23,18,23
#define DQP4  16,20,16,20, 20,25,20,25
#define DQP5  18,23,18,23, 23,29,23,29


/****************************************************************************
  Non static functions
****************************************************************************/
core_block core_block_multiply(core_block a, core_block b)
{
  int32_t i, j, k;
  int32_t sum;
  core_block res;

  for (i = 0; i < 4; ++i)
    for (j = 0; j < 4; ++j) {
      sum = 0;
      for (k = 0; k < 4; ++k)
	sum += CoreBlock(a, i, k) * CoreBlock(b, k, j);
      CoreBlock(res, i, j) = sum;
    }
  return res;
}

core_block forward_core_transform(core_block original)
{
  static const core_block transform0 =
    { {1, 1, 1, 1, 2, 1, -1, -2, 1, -1, -1, 1, 1, -2, 2, -1}
    };
  static const core_block transformT =
    { {1, 2, 1, 1, 1, 1, -1, -2, 1, -1, -1, 2, 1, -2, 1, -1}
    };

  core_block temp = core_block_multiply(transform0, original);
  return core_block_multiply(temp, transformT);
}

core_block forward_quantize(core_block raw, int32_t quantizer,
			    int32_t rounding_mode)
{
  static const core_block factors[6] =
    { {{QP0, QP0}}, {{QP1, QP1}}, {{QP2, QP2}},
      {{QP3, QP3}}, {{QP4, QP4}}, {{QP5, QP5}}
    };
  int32_t qbits = (quantizer / 6) + 15;
  int32_t table = (quantizer % 6);
  int32_t round_adjust = (1 << qbits) / rounding_mode;
  int32_t l;
  core_block res;

  for (l = 0; l < 16; ++l) {
    int32_t value = raw.items[l];
    res.items[l] = CombineSign(ExtractSign(value),
			       (abs(value) * factors[table].items[l] +
				round_adjust) >> qbits);
  }
  return res;
}

core_block inverse_quantize(core_block quantized, int32_t quantizer,
			    int32_t without_dc)
{
  static const core_block factors[6] =
    { {{DQP0, DQP0}}, {{DQP1, DQP1}}, {{DQP2, DQP2}},
      {{DQP3, DQP3}}, {{DQP4, DQP4}}, {{DQP5, DQP5}}
    };
  int32_t qbits = quantizer / 6;
  int32_t table = quantizer % 6;
  int32_t l;
  core_block res;

  if (without_dc)
    res.items[0] = quantized.items[0];
  for (l = without_dc; l < 16; ++l) {
    int32_t value = quantized.items[l];
    res.items[l] = CombineSign(ExtractSign(value),
			       (abs(value) *
				factors[table].items[l]) << qbits);
  }
  return res;
}

core_block inverse_core_transform_slow(core_block coeff)
{
  static const core_block transform0 =
    { {2, 2, 2, 1, 2, 1, -2, -2, 2, -1, -2, 2, 2, -2, 2, -1}
    };
  static const core_block transformT =
    { {2, 2, 2, 2, 2, 1, -1, -2, 2, -2, -2, 2, 1, -2, 2, -1}
    };

  core_block temp = core_block_multiply(transform0, coeff);
  core_block scaled = core_block_multiply(temp, transformT);
  int32_t l;
  for (l = 0; l < 16; ++l) {
    scaled.items[l] = (scaled.items[l] + 128) >> 8;
  }
  return scaled;
}


core_block inverse_core_transform_fast(core_block coeff)
{
  core_block temp = coeff;
  int32_t i, e0, e1, e2, e3, *l = &temp.items[0];
  for (i = 0; i < 4; ++i) {
    e0 = l[0] + l[2];
    e1 = l[0] - l[2];
    e2 = (l[1] >> 1) - l[3];
    e3 = l[1] + (l[3] >> 1);
    *l++ = e0 + e3;
    *l++ = e1 + e2;
    *l++ = e1 - e2;
    *l++ = e0 - e3;
  }
  for (i = 0; i < 4; ++i) {
    l = &temp.items[i];
    e0 = l[0] + l[8];
    e1 = l[0] - l[8];
    e2 = (l[4] >> 1) - l[12];
    e3 = l[4] + (l[12] >> 1);
    l[0] = (e0 + e3 + 32) >> 6;
    l[4] = (e1 + e2 + 32) >> 6;
    l[8] = (e1 - e2 + 32) >> 6;
    l[12] = (e0 - e3 + 32) >> 6;
  }
  return temp;
}

static inline void enter(unsigned char *ptr, int32_t q_delta)
{
  register int32_t i = *ptr + ((q_delta + 32) >> 6);
#ifdef USE_X86_ASM
  /*  asm(" \ */
  /*     cdq;              \ */
  /*     notl %%edx;       \ */
  /*     andl %%edx,%%eax; \ */
  /*     subl $256, %%eax; \ */
  /*     cdq;              \ */
  /*     notl %%edx;       \ */
  /*     orl  %%edx,%%eax; \ */
  /*   ":"=a"(i):"a"(i):"%edx"); */
  /*     *ptr = i; */
  *ptr = Clip(i);
#else
  *ptr = Clip(i);
#endif
}

void direct_ict(core_block coeff, unsigned char *img, int32_t pitch)
{
  core_block temp = coeff;
  int32_t i, e0, e1, e2, e3, *l = &temp.items[0];
  for (i = 0; i < 4; ++i) {
    e0 = l[0] + l[2];
    e1 = l[0] - l[2];
    e2 = (l[1] >> 1) - l[3];
    e3 = l[1] + (l[3] >> 1);
    *l++ = e0 + e3;
    *l++ = e1 + e2;
    *l++ = e1 - e2;
    *l++ = e0 - e3;
  }
  for (i = 0; i < 4; ++i) {
    l = &temp.items[i];
    e0 = l[0] + l[8];
    e1 = l[0] - l[8];
    e2 = (l[4] >> 1) - l[12];
    e3 = l[4] + (l[12] >> 1);
    enter(&img[0], e0 + e3);
    enter(&img[pitch], e1 + e2);
    enter(&img[pitch << 1], e1 - e2);
    enter(&img[pitch * 3], e0 - e3);
    ++img;
  }
}

core_block hadamard(core_block coeff)
{
  static const core_block transform =
    { {1, 1, 1, 1, 1, 1, -1, -1, 1, -1, -1, 1, 1, -1, 1, -1}
    };
  return core_block_multiply(transform,
			     core_block_multiply(coeff, transform));
}


////////////////////////////////////////////////////////////////////////////////

void _dump_core_block(core_block block)
{
  int32_t i, j;
  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j)
      printf("%5d", CoreBlock(block, i, j));
    printf("\n");
  }
  printf("\n");
}

#ifdef BUILD_TESTS

int32_t _test_coretrans(int32_t argc, char *argv[])
{

  core_block test_block =
    { {5, 11, 8, 10, 9, 8, 4, 12, 1, 10, 11, 4, 19, 6, 15, 7} };
  int32_t quantizer = 10;
  int32_t mode = IntraRoundingMode;

  core_block temp;
  printf("Original block:\n");
  _dump_core_block(test_block);
  temp = forward_core_transform(test_block);
  printf("Forward core transform:\n");
  _dump_core_block(temp);
  temp = forward_quantize(temp, quantizer, mode);
  printf("Forward quantizer:\n");
  _dump_core_block(temp);
  temp = inverse_quantize(temp, quantizer, 0);
  printf("Inverse quantizer:\n");
  _dump_core_block(temp);
  temp = inverse_core_transform(temp);
  printf("Inverse core transform:\n");
  _dump_core_block(temp);

  return 0;
}

#endif
