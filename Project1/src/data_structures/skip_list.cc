#include "../../include/data_structures/skip_list.hpp"


SkipList::SkipList(const uint16_t& _max_level, const double& p): max_level(_max_level),
                                                                 expand_up_probability(p),
                                                                 data_head(NULL),
                                                                 size(0)
{
  skip_heads = new SkipListNode*[max_level - 1];
  for (size_t i = 0; i < max_level - 1; i++)
    skip_heads[i] = NULL;
}


SkipList::~SkipList(void)
{
  /* do later */

  // for (size_t i = 0; i < max_level - 1; i++)
  // {
  //   if (skip_heads[i])
  //     delete skip_heads[i];
  // }
}


void SkipList::insert(Record* record, const std::string& date)
{

}
