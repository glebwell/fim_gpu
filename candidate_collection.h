#ifndef CANDIDATE_COLLECTION_
#define CANDIDARE_COLLECTION_

#include <string>
#include <map>

using namespace std;

class frontier_node;

class candidate_collection
{
    std::map<frontier_node*, float> m_cand_map;
    pthread_mutex_t m_mutex;
    unsigned char m_max_cand_length;

public:
    candidate_collection();
    ~candidate_collection();
    void store(frontier_node *n, float support);
    void print_candidates(const string&filename) const;
    const std::map<frontier_node *, float> &get_map() const;
    std::map<frontier_node *, float> &get_map();
    unsigned char get_max_cand_len() const;
    float get_cand_support(unsigned int* candidate, unsigned int len) const;
    void filter_candidates_by_quality(float confidense);
    void append_rules_to_file(const string& filename);
    float get_average_quality();
    float get_max_quality() const;
private:
    bool is_cand_equal(unsigned int* a, unsigned int len_a, unsigned int* b, unsigned int len_b) const;

};

#endif // CANDIDATE_COLLECTION_
