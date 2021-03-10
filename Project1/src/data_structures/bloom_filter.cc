#include <cstring>
#include "../../include/data_structures/bloom_filter.hpp"


BloomFilter::BloomFilter(const uint64_t& number_of_bytes, const uint16_t& K):
size(number_of_bytes), num_hashes(K)
{
  bits = std::make_unique<uint8_t[]>(number_of_bytes);
  std::memset(bits.get(), 0, number_of_bytes);
}


BloomFilter::~BloomFilter(void)
{
  bits = NULL;
}


uint64_t BloomFilter::djb2(const std::string& str)
{
	uint64_t hash = 5381;
	int c = 0;
  size_t i = 0;

	while (c = str[i++])
		hash = ((hash << 5) + hash) + c;

	return hash;
}


uint64_t BloomFilter::sdbm(const std::string& str)
{
	uint64_t hash = 0;
	int c = 0;
  size_t i = 0;

	while (c = str[i++])
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}


uint64_t BloomFilter::hash_i(const uint64_t& djb2_hash, const uint64_t& sdbm_hash, const size_t& i)
{
	return djb2_hash + i * sdbm_hash + i * i;
}


void BloomFilter::SetBit(const uint64_t n)
{
  bits[n / 8] |= 1 << ((8 - 1) - n % 8);
}


bool BloomFilter::GetBit(const uint64_t n)
{
  return (bits[n / 8] & (1 << (8 - 1) - n % 8)) != 0;
}


void BloomFilter::insert(const std::string& str)
{
  uint64_t djb2_hash = djb2(str);
  uint64_t sdbm_hash = sdbm(str);
  uint64_t M = size * 8;

  for (size_t i = 0; i < num_hashes; i++)
    SetBit(hash_i(djb2_hash, sdbm_hash, i) % M);
}


bool BloomFilter::is_probably_in(const std::string& str)
{
  uint64_t djb2_hash = djb2(str);
  uint64_t sdbm_hash = sdbm(str);
  uint64_t M = size * 8;

  for (size_t i = 0; i < num_hashes; i++)
    if (!GetBit(hash_i(djb2_hash, sdbm_hash, i) % M))
      return false;

  return true;
}
