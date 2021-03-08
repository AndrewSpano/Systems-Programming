#include <iostream>
#include <string>

#include "../include/data_structures/index.hpp"
#include "../include/utils/parsing.hpp"


int main(int argc, char* argv[])
{

  /*
  std::string s = "1234";
  std::string a = "234";
  bool b = s < a;

  uint16_t s1 = stoi(s);
  uint16_t a1 = stoi(a);
  bool b1 = s1 < a1;

  std::cout << "bool b is " << b << '\n';
  std::cout << "bool b1 is " << b1 << '\n';


  Record r1 = Record(100, "John", "pap", "Albania", 15);
  Record r2 = Record(200, "Bill", "pap", "Albania", 15);
  Record r3 = Record(20, "Jack", "Daniels", "Greece", 42);
  Record r4 = Record(34, "Demi", "lovato", "Romania", 29);

  std::cout << "res: " << (r1 < r2) << '\n';
  std::cout << "res: " << (r1 < 120) << '\n';
  std::cout << "r1 = " << r1 << '\n';

  SortedList<Record>* l = new SortedList<Record>();

  l->insert(&r1);
  l->insert(&r2);
  l->insert(&r3);
  l->insert(&r4);
  l->print();
  */

  /* index struct that will contain all the data structures grouped together */
  Index index;

  parsing::dataset::parse_dataset(argv[2], index);

  return EXIT_SUCCESS;
}
