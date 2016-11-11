#ifndef CANDIDATE_COLLECTION_
#define CANDIDARE_COLLECTION_

#include "string"
#include "map"

#include "frontier_node.h"

using namespace std;

class candidate_collection : public map<frontier_node*, float> {
 public:
  virtual ~candidate_collection();
  void print_candidate(string filename);
  void append_candidate(string filename);
};

class new_candidate_collection
{
    std::map<frontier_node*, float> m_cand_map;
    pthread_mutex_t m_mutex;
    unsigned char m_max_cand_length;
public:
    new_candidate_collection();
    ~new_candidate_collection();
    void store(frontier_node *n, float support);
    void print_candidates(const string&filename) const;
    const std::map<frontier_node *, float> &get_map() const;
    unsigned char get_max_cand_len() const;
    float get_cand_support(unsigned int* candidate, unsigned int len) const;
private:
    bool is_cand_equal(unsigned int* a, unsigned int len_a, unsigned int* b, unsigned int len_b) const;

};

#endif // CANDIDATE_COLLECTION_
