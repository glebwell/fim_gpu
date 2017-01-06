#include <algorithm>
#include <iostream>
#include <fstream>
#include <iostream>

#include "frontier_node.h"
#include "candidate_collection.h"

using namespace std;



candidate_collection::candidate_collection():m_max_cand_length(0)
{
    pthread_mutex_init(&m_mutex, NULL);

}

candidate_collection::~candidate_collection()
{
    map<frontier_node*, float>::iterator it;
    for (it = m_cand_map.begin(); it != m_cand_map.end(); ++it)
      delete it->first;
    pthread_mutex_destroy(&m_mutex);
}

void candidate_collection::store(frontier_node* n, float support)
{
    pthread_mutex_lock(&m_mutex);
    if (m_max_cand_length < n->candidate_len)
        m_max_cand_length = n->candidate_len;
    m_cand_map[n] = support;
    pthread_mutex_unlock(&m_mutex);
}

void candidate_collection::print_candidates(const string& filename) const
{
    map<frontier_node*, float>::const_iterator it, end = m_cand_map.end();
    ofstream of(filename.c_str(),ios::out);
    for (it = m_cand_map.begin(); it != end; ++it)
    {        
        for (int j = 1; j < it->first->candidate_len; j++)
        {
            of << it->first->candidate[j] << " ";
        }
        of << "(" << it->second << ")" << endl;
    }
    of.close();
}

const std::map<frontier_node *, float> &candidate_collection::get_map() const
{
    return m_cand_map;
}

std::map<frontier_node *, float> &candidate_collection::get_map()
{
    return m_cand_map;
}

unsigned char candidate_collection::get_max_cand_len() const
{
    return m_max_cand_length;
}

float candidate_collection::get_cand_support(unsigned int* candidate, unsigned int len) const
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

void candidate_collection::filter_candidates_by_quality(float conf)
{
    map<frontier_node*, float>::const_iterator it, end = m_cand_map.end();
    frontier_node* curr = nullptr;
    for (it = m_cand_map.begin(); it != end; )
    {
        curr = it->first;
        if (curr->quality < conf)
        {
            it = m_cand_map.erase(it);
        }
        else
        {
            ++it;
        }
    }

    //
    // update covered transactions set
    //

    for (const std::pair<frontier_node*, float>& pair : m_cand_map)
    {
        pair.first->update_transaction_set();
    }

}

void candidate_collection::append_rules_to_file(const string &filename)
{
    std::ofstream of(filename.c_str(),ios::out|ios::app);
    of << "Generated rules:\n";
    frontier_node* curr = nullptr;
    for (const std::pair<frontier_node*, float>& pair : m_cand_map)
    {
        of << "IF ";
        curr = pair.first;
        for (int i = 1; i < curr->candidate_len; ++i)
        {
             of << curr->candidate[i] << " ";
        }
        of << "THEN CLASS = " << curr->class_value;
        of << " [ " << curr->quality << " ]\n";
    }


    of << "Total rules: " << m_cand_map.size()
       << " avg_quality = " << get_average_quality()
       << " max_quality = " << get_max_quality()
       << "\n";

}

float candidate_collection::get_average_quality()
{
    size_t total = m_cand_map.size();
    float sum_quality = 0;
    for (const auto& pair : m_cand_map)
    {
        sum_quality += pair.first->quality;
    }

    return sum_quality / total;
}

float candidate_collection::get_max_quality() const
{
    float quality, max_quality = 0;
    for (const auto& pair : m_cand_map)
    {
        quality = pair.first->quality;
        if ( quality > max_quality )
            max_quality = quality;
    }

    return max_quality;
}

bool candidate_collection::is_cand_equal(unsigned int* a, unsigned int len_a, unsigned int* b, unsigned int len_b ) const
{
    if (len_a != len_b)
        return false;

    return std::equal(a, a + len_a, b);
}
