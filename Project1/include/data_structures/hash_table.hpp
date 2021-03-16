#ifndef _HASH_TABLE
#define _HASH_TABLE

#define DEFAULT_NUM_BUCKETS 50


#include <iostream>


typedef struct Bucket* BucketPtr;
typedef struct Bucket
{
  std::string* data = NULL;
  int id = -1;
  BucketPtr next = NULL;

  Bucket(const std::string& _data, const int& _id, BucketPtr _next): id(_id), next(_next)
  {
    data = new std::string(_data);
  }

  ~Bucket(void)
  {
    delete data;
    data = NULL;
    if (next)
      delete next;
  }
} Bucket;



class HashTable
{
  private:

    BucketPtr* buckets;
    uint32_t num_buckets;
    uint64_t size;

    uint64_t _hash_function(const std::string& data);
    uint64_t _bucket_chain_len(const uint32_t& bucket_id);
    void _print_bucket_chain(const uint32_t& bucket_id);

  public:

    explicit HashTable(const uint32_t& num_buckets);
    ~HashTable(void);

    uint64_t get_size(void);

    void insert_if_not_exists(const std::string& data);
    int get_id(const std::string& data);

    std::string** build_id_to_country_lookup(void);

    void print_bucket_lens(void);
    void print_bucket_chains(void);
};


#endif
