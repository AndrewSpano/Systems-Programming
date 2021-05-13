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
        uint8_t* bits;

        uint64_t _djb2(const std::string & str);
        uint64_t _sdbm(const std::string & str);
        uint64_t _hash_i(const uint64_t & djb2_hash, const uint64_t & sdbm_hash, const size_t & i);

        void _set_bit(const uint64_t n);
        bool _get_bit(const uint64_t n);

    public:

        BloomFilter(const uint64_t & number_of_bytes, const uint16_t & K);
        BloomFilter(const uint64_t & number_of_bytes, const uint16_t & K, uint8_t* _bits);
        ~BloomFilter(void);

        BloomFilter* copy(void);

        void insert(const std::string & str);
        bool is_probably_in(const std::string & str);

        uint8_t* get_bloom_filter(void);
        void update(uint8_t update_bits[]);
};


typedef struct BFPair
{
    std::string virus_name = "";
    BloomFilter* bloom_filter = NULL;

    BFPair(const std::string & _virus_name, const uint64_t & bf_size, const uint16_t & K, uint8_t* bits): virus_name(_virus_name)
    { bloom_filter = new BloomFilter(bf_size, K, bits); }

    BFPair(const std::string & _virus_name, BloomFilter* bf): virus_name(_virus_name)
    { bloom_filter = bf->copy(); }

    ~BFPair(void)
    { if (bloom_filter) delete bloom_filter; }

    bool operator < (const std::string & vn)
    { return virus_name < vn; }

    bool operator <= (const std::string & vn)
    { return virus_name <= vn; }

    bool operator == (const std::string & vn)
    { return virus_name == vn; }

    bool operator != (const std::string & vn)
    { return virus_name != vn; }

    bool operator > (const std::string & vn)
    { return virus_name > vn; }

    bool operator >= (const std::string & vn)
    { return virus_name >= vn; }

    bool operator < (const BFPair & bfp)
    { return virus_name < bfp.virus_name; }

    bool operator <= (const BFPair & bfp)
    { return virus_name <= bfp.virus_name; }

    bool operator == (const BFPair & bfp)
    { return virus_name == bfp.virus_name; }

    bool operator != (const BFPair & bfp)
    { return virus_name != bfp.virus_name; }

    bool operator > (const BFPair & bfp)
    { return virus_name > bfp.virus_name; }

    bool operator >= (const BFPair & bfp)
    { return virus_name >= bfp.virus_name; }
} BFPair;


#endif
