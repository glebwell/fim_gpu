#include <sstream>
#include <fstream>
#include <iostream>
#include "rule_generator.h"
#include "candidate_collection.h"

rule_generator::rule_generator(float conf, const new_candidate_collection &cand_ref): m_conf(conf), m_cand_coll(cand_ref)
{
    const std::map<frontier_node*, float>& cand = m_cand_coll.get_map();
    unsigned char max_len = m_cand_coll.get_max_cand_len();
    std::map<frontier_node*, float>::const_iterator it, end = cand.end();
    frontier_node* curr = NULL;
    for (it = cand.begin(); it != end; ++it)
    {
        curr = it->first;
        if (curr->candidate_len == max_len)
        {
            m_best_candidates.push_back(curr);
        }

    }
}

void rule_generator::append_rules_to_file(const string &filename) const
{
    std::ofstream of(filename.c_str(),ios::out|ios::app);
    of << "Generated rules:\n";

    for (const auto& r : m_rules)
        of << rule_to_string(r) << "\n";
}

void rule_generator::generate()
{
    if (m_rules.empty())
    {
        const unsigned char max_len = m_cand_coll.get_max_cand_len();
        float curr_conf;
        float rule_condition_sup;
        float rule_result_sup;
        unsigned int tmp_array[128] = {0};
        int k;


        for (const auto& cand : m_best_candidates)
        {
            for (unsigned int i = 2; i < max_len - 1; ++i ) // start with len = 2, because of first element is zero(dummy candidate)
            {
                std::cout << "candidate: ";
                for (int j = 0; j < cand->candidate_len; ++j )
                    std::cout << cand->candidate[j] << " ";
                std::cout << "\n";


                std::cout << "candidate condition: ";
                for (int j = 0; j < i; ++j )
                    std::cout << cand->candidate[j] << " ";
                std::cout << "\n";



                rule_condition_sup = m_cand_coll.get_cand_support(cand->candidate, i);

                k = i;
                for (int j = 1; k < max_len - i + 2; ++k, ++j)
                {
                    tmp_array[j] = cand->candidate[k];
                }

                std::cout << "candidate result: ";
                for (int j = 0; j < max_len - i + 1; ++j )
                    std::cout << tmp_array[j] << " ";
                std::cout << "\n";

                rule_result_sup = m_cand_coll.get_cand_support(tmp_array, max_len - i + 1);
                if ( rule_condition_sup && rule_result_sup ) // if == 0 no cand founded
                {
                    curr_conf = rule_result_sup / rule_condition_sup;
                    if ( curr_conf > m_conf )
                    {
                        std::cout << "confidence = " << curr_conf << "\n";
                        while ( i < max_len - 1 ) // add all possible rules
                        {
                            save_rule(cand->candidate, i, cand->candidate + i, max_len - i);
                            ++i;
                        }
                    }
                }
            }
        }
    }

}

void rule_generator::save_rule(unsigned int *rule_condition, unsigned int rc_len, unsigned int *rule_result, unsigned int rr_len)
{
    vec_int rule_condition_v(rule_condition, rule_condition + rc_len);
    vec_int rule_result_v(rule_result, rule_result + rr_len);

    m_rules.emplace_back(std::move(rule_condition_v), std::move(rule_result_v));

}

string rule_generator::rule_to_string(const rule_generator::rule &r) const
{
    std::stringstream str;
    str << "IF ";
    for (int i = 1; i < r.first.size(); ++i) // skip dummy candidate zero
    {
        str << r.first[i];
        str << " ";
    }
    str << "THEN ";
    for (int i = 1; i < r.second.size(); ++i) // skip dummy candidate zero
    {
        str << r.second[i];
        str << " ";
    }
    return str.str();
}
