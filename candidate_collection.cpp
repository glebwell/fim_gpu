#include <algorithm>
#include "candidate_collection.h"
#include <iostream>
#include "fstream"
#include "iostream"

using namespace std;

class frontier_node;

candidate_collection::~candidate_collection()
{
  map<frontier_node*, float>::iterator it;
  for (it = begin(); it != end(); it++)
    delete it->first;
  clear();
}

void candidate_collection::print_candidate(string filename)
{
  map<frontier_node*, float>::iterator it;
  ofstream of(filename.c_str(),ios::out);

  for (it = begin(); it != end(); ++it) {
    for (int j = 1; j < it->first->candidate_len; j++)
    {
      of << it->first->candidate[j] << " ";
    }
    of << "(" << it->second << ")" << endl;
  }
  of.close();
}

void candidate_collection::append_candidate(string filename) {
  int i,j;
  map<frontier_node*, float>::iterator it;
  ofstream of(filename.c_str(),ios::out|ios::app);

  for (it = begin(); it != end(); it++) {
    for (j = 1; j < it->first->candidate_len; j++) {
      of << it->first->candidate[j] << " ";
    }
    of << "(" << it->second << ")" << endl;
  }
  of.close();
}

new_candidate_collection::new_candidate_collection():m_max_cand_length(0)
{
    pthread_mutex_init(&m_mutex, NULL);

}

new_candidate_collection::~new_candidate_collection()
{
    map<frontier_node*, float>::iterator it;
    for (it = m_cand_map.begin(); it != m_cand_map.end(); ++it)
      delete it->first;
    pthread_mutex_destroy(&m_mutex);
}

void new_candidate_collection::store(frontier_node* n, float support)
{
    pthread_mutex_lock(&m_mutex);
    if (m_max_cand_length < n->candidate_len)
        m_max_cand_length = n->candidate_len;
    m_cand_map[n] = support;
    pthread_mutex_unlock(&m_mutex);
}

void new_candidate_collection::print_candidates(const string& filename) const
{
    map<frontier_node*, float>::const_iterator it, end = m_cand_map.end();
    ofstream of(filename.c_str(),ios::out);
    for (it = m_cand_map.begin(); it != end; ++it)
    {
        if (m_max_cand_length == it->first->candidate_len)
        {
            for (int j = 1; j < it->first->candidate_len; j++)
            {
                of << it->first->candidate[j] << " ";
            }
            of << "(" << it->second << ")" << endl;
        }
    }
    of.close();
}

const std::map<frontier_node *, float> &new_candidate_collection::get_map() const
{
    return m_cand_map;
}

unsigned char new_candidate_collection::get_max_cand_len() const
{
    return m_max_cand_length;
}

float new_candidate_collection::get_cand_support(unsigned int* candidate, unsigned int len) const
{
     map<frontier_node*, float>::const_iterator it, end = m_cand_map.end();
     frontier_node* node = NULL;
     std::cout << "get_cand_support: ";
     for (unsigned int i = 0; i < len; ++i)
        std::cout << candidate[i] << " ";
     std::cout << "\n";
     for (it = m_cand_map.begin(); it != end; ++it)
     {
         node = it->first;
         if ( is_cand_equal(candidate, len, node->candidate, node->candidate_len) )
             return node->support;
     }

     return 0;
}

bool new_candidate_collection::is_cand_equal(unsigned int* a, unsigned int len_a, unsigned int* b, unsigned int len_b ) const
{
    if (len_a != len_b)
        return false;

    return std::equal(a, a + len_a, b);
}
