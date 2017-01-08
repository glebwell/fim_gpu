#ifndef FRONTIER_PREEXPAND_H_
#define FRONTIER_PREEXPAND_H_

#include <vector>
#include <string>

#include "mem_controller.h"

class frontier_node;
class CPUMemPool;
class GPUMemPool;
class job_manager;

class frontier_preexpand {
 public:
  frontier_node** base;
  frontier_node** stack_pointer;
 public:
  CPUMemPool * cmc;
  int data_size;

  int fim_num;
  float support_ratio;
  static std::vector<int> transaction_classes;
  static unsigned char classes_amount;
  void pre_expand_init(CPUMemPool *pcmc, const std::string& file, const string &class_file, float min_sup);

  void produce_jobs(job_manager &jm, int threshold);
  void destroy();
  void debug();
};

#endif /* FRONTER_STACK_PREEXPAND_H_ */
