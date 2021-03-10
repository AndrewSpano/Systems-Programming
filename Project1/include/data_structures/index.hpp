#ifndef _INDEX
#define _INDEX

#include "record.hpp"
#include "record_list.hpp"
#include "skip_list.hpp"
#include "disease_list.hpp"


typedef struct Index
{
  RecordList* records = NULL;

  /* skips lists */
  /* bloom filters */

  Index(void)
  {
    records = new RecordList();
  }

  ~Index(void)
  {
    if (records)
    {
      delete records;
      records = NULL;
    }
  }

  void insert(Record* existing_record, Record* new_record, const std::string& disease="",
              const std::string& status="", const std::string& date="")
  {
    /* if the record has not been seen before, insert it in the list with records */
    if (!existing_record)
    {
      records->insert(new_record);
      existing_record = new_record;
    }
    else
    {
      delete new_record;
    }




  }

} Index;



#endif
