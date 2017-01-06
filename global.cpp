/*
 * global.cpp
 *
 *  Created on: May 24, 2011
 *      Author: zhangfan
 */
#include "global.h"

float time_support_counting = 0;
float time_memory_operation = 0;
float time_candidate_generation = 0;
float time_init = 0;
float time_expansion = 0;

candidate_collection cand_coll;

void set_bit(unsigned int * vlist, int index, int data) {
  int seg = index / (sizeof(unsigned int) * 8);
  int offset = index % (sizeof(unsigned int) * 8);
  unsigned int bit_mask = 0x80000000;
  bit_mask = bit_mask>>offset;
  if (data == 1) {
    vlist[seg] = vlist[seg] | bit_mask;
  } else if (data == 0) {
    vlist[seg] = vlist[seg] & (~bit_mask);
  }
}

int get_bit(unsigned int * vlist, int index) {
  int seg = index / (sizeof(unsigned int) * 8);
  int offset = index % (sizeof(unsigned int) * 8);

  unsigned int bit_mask = 0x80000000;
  bit_mask = bit_mask >> offset;

  if ((vlist[seg] & bit_mask) == 0) {
    return 0;
  } else {
    return 1;
  }
}

int bitcnt(unsigned int src) {
  src = (src & 0x55555555) + ((src >> 1) & 0x55555555);
  src = (src & 0x33333333) + ((src >> 2) & 0x33333333);
  src = (src & 0x0f0f0f0f) + ((src >> 4) & 0x0f0f0f0f);
  src = (src & 0x00ff00ff) + ((src >> 8) & 0x00ff00ff);
  src = (src & 0x0000ffff) + ((src >> 16) & 0x0000ffff);
  return src;
}

