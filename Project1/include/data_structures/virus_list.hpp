#ifndef _DISEASE_LIST
#define _DISEASE_LIST

#include "record.hpp"
#include "skip_list.hpp"
#include "bloom_filter.hpp"


typedef struct VirusNode* VirusNodePtr;
typedef struct VirusNode
{
  std::string virus_name = "";
  SkipList* vaccinated = NULL;
  SkipList* non_vaccinated = NULL;
  BloomFilter* bloom_filter = NULL;
  VirusNodePtr next = NULL;

  VirusNode(const std::string& _virus_name, VirusNodePtr _next, const uint64_t& bloom_filter_size):
  virus_name(_virus_name), next(_next)
  {
    vaccinated = new SkipList(DEFAULT_MAX_LEVEL, DEFAULT_PROBABILITY);
    non_vaccinated = new SkipList(DEFAULT_MAX_LEVEL, DEFAULT_PROBABILITY);
    bloom_filter = new BloomFilter(bloom_filter_size, DEFAULT_K);
  }

  ~VirusNode(void)
  {
    delete bloom_filter;
    delete vaccinated;
    delete non_vaccinated;

    virus_name = "";
    next = NULL;
  }

  void insert(const bool& status, Record* record, const std::string& date)
  {
    if (status)
    {
      vaccinated->insert(record, date);
      bloom_filter->insert(record->id);
    }
    else
    {
      non_vaccinated->insert(record, date);
    }
  }

} VirusNode;


class VirusList
{
  private:

    VirusNodePtr head;
    uint32_t size;
    const uint64_t bloom_filter_size;

    void _delete_data(void);

  public:

    VirusList(const uint64_t& _bloom_filter_size);
    ~VirusList(void);

    void insert(Record* record, const std::string& virus_name, const bool& status, const std::string& date);
    bool exists_in_virus_name(Record* record, const std::string& virus_name);

    bool probably_in_bloom_filter_of_virus(const std::string& id, const std::string& virus_name);
    void vaccine_status(const std::string& id, const std::string& virus_name);
};


#endif
