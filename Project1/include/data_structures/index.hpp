#ifndef _INDEX
#define _INDEX

#include <iostream>
#include <ctime>

#include "record_list.hpp"
#include "virus_list.hpp"
#include "hash_table.hpp"
#include "../utils/macros.hpp"
#include "../utils/utils.hpp"


typedef struct Index
{
  RecordList* records_list = NULL;
  HashTable* countries_hash_table = NULL;
  VirusList* virus_list = NULL;

  Index(const uint64_t& _bloom_filter_size, const uint32_t& hash_table_num_buckets)
  {
    records_list = new RecordList();
    countries_hash_table = new HashTable(hash_table_num_buckets);
    virus_list = new VirusList(_bloom_filter_size);
  }

  ~Index(void)
  {
    delete records_list;
    records_list = NULL;

    delete countries_hash_table;
    countries_hash_table = NULL;

    delete virus_list;
    virus_list = NULL;
  }

  void insert(Record* existing_record, Record* new_record, const std::string& virus_name="",
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
    virus_list->insert(new_record, virus_name, status, date);

    /* insert the country to the hast table with the countries, if it does not exist already */
    countries_hash_table->insert_if_not_exists(new_record->country);
  }

  void insert_citizen_record(const std::string& id, const std::string& first_name,
                             const std::string& last_name, const std::string& country,
                             const uint8_t& age, const std::string& virus_name,
                             const bool& status, const std::string& date)
  {
    Record* new_record = new Record(id, first_name, last_name, country, age);
    Record* same_id_record = records_list->get(id);

    /* check if a record with the same ID exists, and make sure it does not create any conflict */
    if (same_id_record && !parsing::processing::is_valid_new_record(new_record, same_id_record))
      DELETE_LOG_AND_RETURN(new_record, *new_record);

    std::string date_vaccinated = virus_list->get_vaccination_date(id, virus_name);
    if (date_vaccinated != "")
      DELETE_LOG_VACCINATION_AND_RETURN(new_record, date_vaccinated)

    bool exists_in_non_vaccinated = virus_list->exists_in_virus_name(id, virus_name, false, true);
    if (!status && exists_in_non_vaccinated)
      DELETE_LOG_NON_VACCINATION_AND_RETURN(new_record)

    /* if we get here it means that the record is legit, and therefore insert it in the database */
    if (exists_in_non_vaccinated)
      virus_list->remove_from_non_vaccinated(id, virus_name);
    std::string status_as_string = (status) ? "YES" : "NO";
    insert(same_id_record, new_record, virus_name, status_as_string, date);
  }

  void vaccinate_now(const std::string& id, const std::string& first_name,
                     const std::string& last_name, const std::string& country,
                     const uint8_t& age, const std::string& virus_name)
  {
    Record* new_record = new Record(id, first_name, last_name, country, age);
    Record* same_id_record = records_list->get(id);

    /* check if a record with the same ID exists, and make sure it does not create any conflict */
    if (same_id_record && !parsing::processing::is_valid_new_record(new_record, same_id_record))
      DELETE_LOG_AND_RETURN(new_record, *new_record);

    std::string date_vaccinated = virus_list->get_vaccination_date(id, virus_name);
    if (date_vaccinated != "")
      DELETE_LOG_VACCINATION_AND_RETURN(new_record, date_vaccinated)

    /* if we get here it means that the record is legit, and therefore insert it in the database */
    if (virus_list->exists_in_virus_name(id, virus_name, false, true))
      virus_list->remove_from_non_vaccinated(id, virus_name);
    /* get the status (always yes for vaccinateNow) */
    std::string status_as_string = "YES";
    /* get todays date */
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%d-%m-%Y",timeinfo);
    std::string todays_date(buffer);
    /* finally insert the record */
    insert(same_id_record, new_record, virus_name, status_as_string, todays_date);
  }

} Index;



#endif
