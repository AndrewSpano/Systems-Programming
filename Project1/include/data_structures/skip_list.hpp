#ifndef _SKIP_LIST
#define _SKIP_LIST

#define DEFAULT_MAX_LEVEL 10
#define DEFAULT_PROBABILITY 0.5


#include "record.hpp"


typedef struct SkipListDataNode* SkipListDataNodePtr;
typedef struct SkipListDataNode
{
  SkipListDataNodePtr next;
  Record* data;
  std::string* date;

  SkipListDataNode(SkipListDataNodePtr _next, Record* record, const std::string& _date):
  next(_next), data(record)
  {
    date = new std::string(_date);
  }

  ~SkipListDataNode(void)
  {
    delete date;
  }
} SkipListDataNode;


typedef struct SkipListNode* SkipListNodePtr;
typedef struct SkipListNode
{
  SkipListNodePtr next;
  SkipListNodePtr down;
  SkipListDataNodePtr data_node;

  SkipListNode(SkipListNodePtr _next, SkipListNodePtr _down, SkipListDataNodePtr _data):
  next(_next), down(_down), data_node(_data)
  { }

  ~SkipListNode(void)
  { }
} SkipListNode;


class SkipList
{
  private:
    uint16_t max_level;
    double expand_up_probability;
    SkipListDataNodePtr data_head;
    SkipListNodePtr* skip_heads;
    uint64_t size;

    bool _expand(void);
    void _expand_up(SkipListDataNodePtr data_node, SkipListNodePtr* prev_nodes);
    void _delete_skip_level(const uint16_t& level);
    void _delete_head_data(void);

  public:

    SkipList(const uint16_t& _max_level, const double& p);
    ~SkipList(void);

    void insert(Record* data, const std::string& date);
    Record* get(const std::string& id);
    bool in(const std::string& id);

    void print(void);

};


#endif
