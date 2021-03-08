#ifndef _INDEX
#define _INDEX

#include "record.h"
#include "list.hpp"


typedef struct Index
{
  SortedList<Record>* records = NULL;

  Index(void)
  {
    records = new SortedList<Record>();
  }

  ~Index(void)
  {
    if (records)
    {
      delete records;
      records = NULL;
    }
  }
} Index;



#endif
