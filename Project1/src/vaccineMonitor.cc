#include <iostream>
#include <string>
#include <memory>
#include <cstring>

#include "../include/data_structures/index.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/data_structures/record_list.hpp"


int main(int argc, char* argv[])
{
  std::cout  << std::endl;

  /* arguments */
  std::string dataset_path = "";
  uint64_t bloom_filter_size = 0;
  if (!parsing::arguments::parse_arguments(argc, argv, dataset_path, bloom_filter_size))
  {
    std::cout << std::endl;
    return EXIT_FAILURE;
  }

  /* initialize the seed for the SkipList */
  srand(time(NULL));

  /* index struct that will contain all the data structures grouped together */
  Index index(bloom_filter_size, DEFAULT_NUM_BUCKETS);

  /* parse the dataset and insert information into the data structures */
  parsing::dataset::parse_dataset(dataset_path, index);

  std::cout << "records list:" << '\n';
  index.records_list->print();

  std::cout << std::endl;
  return EXIT_SUCCESS;
}
