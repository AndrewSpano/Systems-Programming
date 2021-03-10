#ifndef _SKIP_LIST
#define _SKIP_LIST

#define DEFAULT_MAX_LEVEL 10
#define DEFAULT_PROBABILITY 0.5


#include "record.hpp"


typedef struct SkipListDataNode
{
  SkipListDataNode* next;
  Record* data;
  std::string* date;
} SkipListDataNode;


typedef struct SkipListNode
{
  SkipListNode* next;
  SkipListNode* down;
  SkipListDataNode* data_node;
} SkipListNode;


class SkipList
{
  private:
    uint16_t max_level;
    double expand_up_probability;
    SkipListDataNode* data_head;
    SkipListNode** skip_heads;
    uint64_t size;

  public:

    SkipList(const uint16_t& _max_level, const double& p);
    ~SkipList(void);

    void insert(Record* record, const std::string& date);

};


#endif
