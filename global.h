/*
 * global.h
 *
 *  Created on: May 24, 2011
 *      Author: zhangfan
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <vector>
#include "time_analysis.h"
#include "candidate_collection.h"

#define MAX_CORE 16

#define MAX_BLOCK 2048
#define MAX_THREAD 256

#define MAX_FRONTIER_SIZE (64*MAX_BLOCK)
#define MAX_LUG_SIZE (16*MAX_BLOCK)
#define MAX_EXPAND_SIZE (16*MAX_BLOCK)

#define JOB_SIZE 8
#define JOB_INTENSITY 64

extern const unsigned long MAX_GPU_MEM_SIZE;
extern const unsigned long MAX_CPU_MEM_SIZE;

extern float time_support_counting;
extern float time_memory_operation;
extern float time_candidate_generation;
extern float time_init;
extern float time_expansion;

extern candidate_collection cand_coll;

using namespace std;

void set_bit(unsigned int * vlist, int index, int data);
int  get_bit(unsigned int * vlist, int index);
int bitcnt(unsigned int src);

#endif /* GLOBAL_H_ */
