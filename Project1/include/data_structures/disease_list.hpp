#ifndef _DISEASE_LIST
#define _DISEASE_LIST

#include "record.hpp"
#include "skip_list.hpp"
#include "bloom_filter.hpp"


typedef struct DiseaseNode* DiseaseNodePtr;

typedef struct DiseaseNode
{
  std::string disease = "";
  SkipList* vaccinated = NULL;
  SkipList* non_vaccinated = NULL;
  BloomFilter* bloom_filter = NULL;
  DiseaseNodePtr next = NULL;

  DiseaseNode(const std::string& _disease, DiseaseNodePtr _next, const uint64_t& bloom_filter_size):
  disease(_disease), next(_next)
  {
    vaccinated = new SkipList(DEFAULT_MAX_LEVEL, DEFAULT_PROBABILITY);
    non_vaccinated = new SkipList(DEFAULT_MAX_LEVEL, DEFAULT_PROBABILITY);
    bloom_filter = new BloomFilter(bloom_filter_size, DEFAULT_K);
  }

  ~DiseaseNode(void)
  {
    /* delete skip lists and bloom filter - clear memory */
    delete bloom_filter;
    delete vaccinated;
    delete non_vaccinated;

    disease = "";
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

} DiseaseNode;


class DiseaseList
{
  private:
    DiseaseNodePtr head;
    uint32_t size;

  public:

    DiseaseList(void);
    ~DiseaseList(void);
    void insert(Record* record, const std::string& disease, const bool& status,
                const std::string& date);
};


#endif
