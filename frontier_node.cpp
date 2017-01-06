/*
 * frontier_node.cpp
 *
 *  Created on: Jul 19, 2011
 *      Author: zhangfan
*/
#include <cuda_runtime.h>
#include <string>
#include <iostream>
#include <cstring>

#include <algorithm>

#include "frontier_node.h"
#include "global.h"
#include "frontier_preexpand.h"

using namespace std;

int frontier_node::vlist_len = 0;
int frontier_node::vlist_len_int = 0;
int frontier_node::vlist_len_int_16 = 0;

std::set<int> frontier_node::covered_transactions_set;

frontier_node::frontier_node() {
  candidate_len = 0;
  support = 0;
  vlist_location = UNALLOCATED;
  quality = 0.f;
  class_value = 255;
  class_distribution.resize(frontier_preexpand::classes_amount);
}

frontier_node::frontier_node(const frontier_node &fn) {
  *this = fn;
}

frontier_node::~frontier_node() {
  candidate_len = 0;
  support = 0;
  vlist_location = UNALLOCATED;
}

frontier_node & frontier_node::operator=(const frontier_node &fn) {
  int i = 0;
  memcpy((void *)candidate,(void *)fn.candidate,
         sizeof(unsigned int) * fn.candidate_len);
  candidate_len = fn.candidate_len;
  support = fn.support;
  quality = fn.quality;
  class_distribution = fn.class_distribution;
  class_value = fn.class_value;
  vlist_mem_ref = fn.vlist_mem_ref;
  vlist_location = fn.vlist_location;

  return *this;
}

void frontier_node::alloc_vlist_cpu(CPUMemPool * cmc=NULL) {
  vlist_location = MAIN_MEMORY;
  vlist_mem_ref.c_addr = cmc->alloc();

  if (vlist_mem_ref.c_addr == NULL) {
    cerr << "Error allocating vlist on CPU" << endl;
    exit(0);
  }
}

void frontier_node::free_vlist_cpu(CPUMemPool * cmc = NULL) {
  if (cmc->free(vlist_mem_ref.c_addr) == NULL) {
    cerr << "Error in free vlist on CPU" << endl;
    exit(0);
  }
  vlist_location = UNALLOCATED;
}

void frontier_node::alloc_vlist_gpu(GPUMemPool * gmc = NULL) {
  int vlist_len_int;
  cudaError_t err;

  vlist_len_int = (vlist_len + 31) / 32;

  vlist_location = GRAPHIC_MEMORY;
  vlist_mem_ref.g_addr = gmc->alloc();

  if (vlist_mem_ref.g_addr == NULL) {
    cerr << "Error in vlist allocation on GPU"<<endl;
    exit(0);
  }
  
  return;
}

void frontier_node::free_vlist_gpu(GPUMemPool * gmc) {
  if (gmc->free(vlist_mem_ref.g_addr) == NULL) {
    cerr << "Error in free vlist on GPU" << endl;
    exit(0);
  }
  vlist_location=UNALLOCATED;
}

void frontier_node::assign_vlist(unsigned int * vlist_cpy) {
  int vlist_len_int;
  cudaError_t err;
  vlist_len_int = (vlist_len + 31) / 32;
  if (vlist_location == MAIN_MEMORY) {
    memcpy((void *)vlist_mem_ref.c_addr, (void *)vlist_cpy,
           sizeof(unsigned int) * vlist_len_int);
  } else if (vlist_location == GRAPHIC_MEMORY) {
    err = cudaMemcpy(vlist_mem_ref.g_addr,vlist_cpy,
                     sizeof(int) * vlist_len_int, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
      cerr << vlist_mem_ref.g_addr << endl;
      cerr << "Error in vlist copy" << endl;
    }
  } else {
    cerr << "Vlist is not allocated" << endl;
  }
  return;
}

void frontier_node::transfer_vlist_ctog(CPUMemPool * cmc,GPUMemPool * gmc) {
  cudaError_t err;
  unsigned int * vlist_tmp;
  unsigned int vlist_len_int = (vlist_len + 31) / 32;

  if (vlist_location != MAIN_MEMORY) {
    cerr << "Node is not in main memory"<<endl;
    exit(0);
  }
  
  vlist_tmp = gmc->alloc();
  if (vlist_tmp == NULL) {
    cerr << "GPU memory pool is full" << endl;
    exit(0);
  }
  err = cudaMemcpy(vlist_tmp,vlist_mem_ref.c_addr,sizeof(int) * vlist_len_int, cudaMemcpyHostToDevice);
  if (err != cudaSuccess) {
    cerr << "Error in cudaMemcpy " << err << endl;
    exit(0);
  }
  
  if (cmc->free(vlist_mem_ref.c_addr) == NULL) {
    cerr << "Error in free vlist on CPU " << err << endl;
    exit(0);
  }
  vlist_location = GRAPHIC_MEMORY;
  vlist_mem_ref.g_addr = vlist_tmp;
}

void frontier_node::transfer_vlist_gtoc(GPUMemPool * gmc, CPUMemPool * cmc) {
  cudaError_t err;
  unsigned int * vlist_tmp;
  unsigned int vlist_len_int = (vlist_len + 31) / 32;

  if (vlist_location != GRAPHIC_MEMORY) {
    cerr << "Node is not in graphic memory" << endl;
    exit(0);
  }
  vlist_tmp = cmc->alloc();
  if (vlist_tmp == NULL) {
    cerr << "CPU memory pool is full" << endl;
    exit(0);
  }

  err = cudaMemcpy(vlist_tmp, vlist_mem_ref.g_addr, sizeof(unsigned int) * vlist_len_int, cudaMemcpyDeviceToHost);
  if (err != cudaSuccess) {
    cerr << "Error in cudaMemcpy " << err << endl;
    exit(0);
  }
  gmc->free(vlist_mem_ref.g_addr);
  vlist_location = MAIN_MEMORY;
  vlist_mem_ref.c_addr = vlist_tmp;
}

void frontier_node::transfer_vlist_ctoc(CPUMemPool * cmc_src,CPUMemPool * cmc_dst) {
  cudaError_t err;
  unsigned int * vlist_tmp;
  unsigned int vlist_len_int = (vlist_len + 31) / 32;

  if (vlist_location != MAIN_MEMORY) {
    cerr << "Node is not in main memory" << endl;
    exit(0);
  }
  
  vlist_tmp = cmc_dst->alloc();
  if (vlist_tmp == NULL) {
    cerr << "CPU memory pool is full" << endl;
    exit(0);
  }

  memcpy((void *)vlist_tmp, (void *)vlist_mem_ref.c_addr, sizeof(unsigned int) * vlist_len_int);

  cmc_src->free(vlist_mem_ref.c_addr);
  vlist_location=MAIN_MEMORY;
  vlist_mem_ref.c_addr=vlist_tmp;
}

void frontier_node::copy_vlist_ctog(CPUMemPool * cmc,GPUMemPool * gmc) {
  cudaError_t err;
  unsigned int * vlist_tmp;
  unsigned int vlist_len_int=(vlist_len+31)/32;

  if (vlist_location != MAIN_MEMORY) {
    cerr << "Node is not in main memory"<<endl;
    exit(0);
  }
  vlist_tmp = gmc->alloc();
  if (vlist_tmp == NULL) {
    cerr << "GPU memory pool is full" << endl;
    exit(0);
  }

  err = cudaMemcpy(vlist_tmp, vlist_mem_ref.c_addr,sizeof(int) * vlist_len_int, cudaMemcpyHostToDevice);
  if (err != cudaSuccess) {
    cerr << "Error in cudaMemcpy " << err << endl;
    exit(0);
  }

  vlist_location = GRAPHIC_MEMORY;
  vlist_mem_ref.g_addr = vlist_tmp;
}

void frontier_node::copy_vlist_gtoc_from_other_frontier_node(CPUMemPool *cmc, const frontier_node *fn)
{
    memcpy((void *)candidate,(void *)fn->candidate,
           sizeof(unsigned int) * fn->candidate_len);
    candidate_len = fn->candidate_len;
    support = fn->support;
    quality = fn->quality;
    class_distribution = fn->class_distribution;
    class_value = fn->class_value;
    vlist_location = fn->vlist_location;

    unsigned int vlist_len_int = (vlist_len + 31) / 32;

    unsigned int * vlist_tmp = cmc->alloc();
    if (vlist_tmp == NULL) {
      cerr << "CPU memory pool is full" << endl;
      exit(0);
    }

    cudaError_t err = cudaMemcpy(vlist_tmp, fn->vlist_mem_ref.g_addr, sizeof(unsigned int) * vlist_len_int, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
       cerr << "Error in cudaMemcpy " << err << endl;
       exit(0);
    }

    vlist_location = MAIN_MEMORY;
    vlist_mem_ref.c_addr = vlist_tmp;
}

void frontier_node::debug() {
  int i = 0, j;
  int vlist_len_int;
  vlist_len_int = (vlist_len + 31) / 32;

  if (candidate_len == 0) {
    cerr << "Divider" << endl;
    return;
  }
  cerr << "item : (";
  for (i = 0; i < candidate_len - 1; i++) {
    cerr << candidate[i] << ",";
  }
  cerr << candidate[i];
  cerr << ") " << " support: " << support << endl;
}

void frontier_node::calculate_distribution(const std::vector<int> &p_transaction_numbers)
{
    const std::vector<int>& tran_cl_vec = frontier_preexpand::transaction_classes;
    transaction_numbers = p_transaction_numbers;
    for (int t : transaction_numbers)
        ++class_distribution[ tran_cl_vec[t] ];
}

void frontier_node::calculate_quality()
{
    if (support != 0)
    {
        auto it = std::max_element(class_distribution.cbegin(), class_distribution.cend());
        class_value = std::distance(class_distribution.cbegin(), it);
        quality = (*it) / (float)support;
    }

}

void frontier_node::update_transaction_set()
{
    if ( covered_transactions_set.size() < vlist_len )
        covered_transactions_set.insert(transaction_numbers.cbegin(), transaction_numbers.cend());
}

void frontier_node::calculate_distribution()
{
    if (support != 0)
    {
        const std::vector<int>& tran_cl_vec = frontier_preexpand::transaction_classes;
        transaction_numbers = get_transaction_numbers();
        for (int t : transaction_numbers)
            ++class_distribution[ tran_cl_vec[t] ];
    }
}


std::vector<int> frontier_node::get_transaction_numbers() const
{
    std::vector<int> result;
    result.reserve(support);
    int current_hits = 0;
    if (vlist_location == MAIN_MEMORY)
    {
        for (int j = 0; j < frontier_node::vlist_len; j++)
        {
          if ( get_bit(vlist_mem_ref.c_addr, j) == 1)
          {
              ++current_hits;
              result.push_back(j);
          }

          if (current_hits == support) // find max hits
              break;
        }
    }

    return result;

}
