#ifndef _RULE_GENERATOR_H_
#define _RULE_GENERATOR_H_

#include <vector>
#include <string>

class new_candidate_collection;
class frontier_node;

class rule_generator
{
    typedef std::vector<unsigned int> vec_int;
    typedef std::pair<vec_int, vec_int> rule;
    float m_conf;
    const new_candidate_collection& m_cand_coll;
    std::vector<frontier_node*> m_best_candidates;
    std::vector<rule> m_rules;
public:
    rule_generator(float conf, const new_candidate_collection &cand_ref);
    void append_rules_to_file(const std::string& filename) const;
    void generate();
private:
    void save_rule(unsigned int* rule_condition, unsigned int rc_len, unsigned int* rule_result, unsigned int rr_len);
    std::string rule_to_string(const rule& r) const;
};

#endif
