/*
 * frontier.h
 *
 *  Created on: May 24, 2011
 *      Author: zhangfan
 */

#ifndef FRONTIER_H_
#define FRONTIER_H_

#include <vector>
#include <pthread.h>
#include <sched.h>

#include "frontier_node.h"
#include "gpu_interface.h"
#include "cpu_interface.h"

class CPUMemPool;
class GPUMemPool;

using namespace std;

class frontier_stack {
 public:
  frontier_node ** base;
  frontier_node ** stack_pointer;
 public:
  GPUMemPool* gmc;
  CPUMemPool* cmc;
  int data_size;
  int fim_num;
  float support_ratio;
  ListUnionGPU lug;
  mem_status vlist_location;

  void init_cpu(CPUMemPool* pcmc, int data_size, float min_sup);

  void copy_to_cpu();
  void copy_to_gpu();
  void transfer_in_cpu(CPUMemPool * new_cmc);

  void expand_gpu(int size, int thread_id);
  void expand_cpu(int size, int thread_id);

  int size() const;

  void destroy();
  void debug();
};

#endif /* FRONTIER_H_ */
