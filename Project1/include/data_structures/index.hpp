#ifndef _INDEX
#define _INDEX

#include "record.hpp"
#include "record_list.hpp"
#include "disease_list.hpp"
#include "hash_table.hpp"


typedef struct Index
{
  RecordList* records_list = NULL;
  HashTable* countries_hash_table = NULL;
  DiseaseList* disease_list = NULL;

  Index(const uint64_t& _bloom_filter_size, const uint32_t& hash_table_num_buckets)
  {
    records_list = new RecordList();
    countries_hash_table = new HashTable(hash_table_num_buckets);
    disease_list = new DiseaseList(_bloom_filter_size);
  }

  ~Index(void)
  {
    delete records_list;
    records_list = NULL;

    delete countries_hash_table;
    countries_hash_table = NULL;

    delete disease_list;
    disease_list = NULL;
  }

  void insert(Record* existing_record, Record* new_record, const std::string& disease="",
              const std::string& _status="", const std::string& date="")
  {
    /* if the record has not been seen before, insert it in the list with records */
    if (!existing_record)
    {
      records_list->insert(new_record);
    }
    else
    {
      delete new_record;
      new_record = existing_record;
    }

    /* insert the new entry in the specified disease list */
    bool status = _status == "YES";
    disease_list->insert(new_record, disease, status, date);

    /* insert the country to the hast table with the countries, if it does not exist already */
    countries_hash_table->insert_if_not_exists(new_record->country);
  }

} Index;



#endif
