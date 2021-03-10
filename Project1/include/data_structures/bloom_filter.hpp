#ifndef _BLOOM_FILTER
#define _BLOOM_FILTER

#define DEFAULT_K 16

#include <iostream>
#include <memory>

class BloomFilter
{
  private:

    uint64_t size;
    uint16_t num_hashes;
    std::unique_ptr<uint8_t[]> bits;

  public:

    BloomFilter(const uint64_t& number_of_bytes, const uint16_t& K);
    ~BloomFilter(void);

    uint64_t djb2(const std::string& str);
    uint64_t sdbm(const std::string& str);
    uint64_t hash_i(const uint64_t& djb2_hash, const uint64_t& sdbm_hash, const size_t& i);

    void SetBit(const uint64_t n);
    bool GetBit(const uint64_t n);

    void insert(const std::string& str);
    bool is_probably_in(const std::string& str);
};


#endif
