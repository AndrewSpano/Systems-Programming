#include <iostream>
#include <string>
#include <memory>
#include <cstring>

#include "../include/data_structures/index.hpp"
#include "../include/utils/parsing.hpp"
#include "../include/data_structures/record_list.hpp"


int main(int argc, char* argv[])
{
  /* index struct that will contain all the data structures grouped together */
  Index index(DEFAULT_NUM_BUCKETS);

  /* initialize the seed for the SkipList */
  srand(time(NULL));

  /* parse the dataset and insert information into the data structures */
  parsing::dataset::parse_dataset(argv[2], index);

  std::cout << "records list:" << '\n';
  index.records_list->print();

  return EXIT_SUCCESS;
}
