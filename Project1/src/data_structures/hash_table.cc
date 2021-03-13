#include <cstring>
#include "../../include/data_structures/hash_table.hpp"


HashTable::HashTable(const uint32_t& _num_buckets): num_buckets(_num_buckets), size(0)
{
  buckets = new BucketPtr[num_buckets];
  std::memset(buckets, '\0', num_buckets * sizeof(BucketPtr));
}


HashTable::~HashTable(void)
{
  for (size_t i = 0; i < num_buckets; i++)
  {
    delete buckets[i];
    buckets[i] = NULL;
  }
  delete[] buckets;
  buckets = NULL;
}


uint64_t HashTable::_hash_function(const std::string& data)
{
  /* implements djb2 hash function */
  uint64_t hash = 5381;
	int c = 0;
  size_t i = 0;

	while (c = data[i++])
		hash = ((hash << 5) + hash) + c;

	return hash;
}


uint64_t HashTable::_bucket_chain_len(const uint32_t& bucket_id)
{
  uint64_t len = 0;
  BucketPtr current_bucket = buckets[bucket_id];

  while (current_bucket)
  {
    len++;
    current_bucket = current_bucket->next;
  }

  return len;
}


void HashTable::_print_bucket_chain(const uint32_t& bucket_id)
{
  BucketPtr current_bucket = buckets[bucket_id];
  while (current_bucket)
  {
    std::cout << *current_bucket->data << ' ';
    current_bucket = current_bucket->next;
  }
  std::cout << std::endl;
}


void HashTable::insert_if_not_exists(const std::string& data)
{
  uint32_t hash = _hash_function(data) % num_buckets;

  if (!buckets[hash])
  {
    buckets[hash] = new Bucket(data, size, NULL);
  }
  else
  {
    BucketPtr current_bucket = buckets[hash];
    while (*current_bucket->data != data && current_bucket->next)
      current_bucket = current_bucket->next;

    /* if item already exists, return */
    if (*current_bucket->data == data)
      return;

    /* else, create a new bucket and insert the data */
    current_bucket->next = new Bucket(data, size, NULL);
  }

  size++;
}


int HashTable::get_id(const std::string& data)
{
  uint32_t hash = _hash_function(data) % num_buckets;
  BucketPtr current_bucket = buckets[hash];

  while (current_bucket && *current_bucket->data != data)
    current_bucket = current_bucket->next;

  return (current_bucket && *current_bucket->data == data) ? current_bucket->id
                                                           : -1;
}


void HashTable::print_bucket_lens(void)
{
  for (size_t i = 0; i < num_buckets; i++)
    std::cout << "Bucket " << (i + 1) << " has length: " << _bucket_chain_len(i) << '\n';
}


void HashTable::print_bucket_chains(void)
{
  for (size_t i = 0; i < num_buckets; i++)
    _print_bucket_chain(i);
}
