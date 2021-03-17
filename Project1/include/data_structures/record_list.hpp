#ifndef _RECORD_LIST
#define _RECORD_LIST

#include "record.hpp"


typedef struct RecordNode* RecordNodePtr;

struct RecordNode
{
  RecordNodePtr next;
  Record* item;

  RecordNode(RecordNodePtr _next, Record* _item)
  {
    next = _next;
    item = _item;
  }

  ~RecordNode(void)
  {
    delete item;
  }
};


class RecordList
{
  private:
    RecordNodePtr head;
    uint64_t size;

    void _remove(size_t pos);
  public:

    RecordList(void);
    ~RecordList(void);

    void insert(Record* item);
    bool in(const std::string& _id);
    Record* get(const std::string& _id);

    void print(void);
};


#endif
