#ifndef _RECORD
#define _RECORD


#include <iostream>


typedef struct Record
{

  uint64_t id = 0;
  std::string name = "";
  std::string surname = "";
  std::string country = "";
  uint8_t age = 0;


  Record(const uint64_t& _id, const std::string& _name, const std::string& _surname,
         const std::string& _country, const int& _age): id(_id), name(_name), surname(_surname),
                                                        country(_country), age(_age)
  { }

  ~Record(void)
  { }

  bool operator < (const uint64_t& _id)
  {
    return id < _id;
  }

  bool operator == (const uint64_t& _id)
  {
    return id == _id;
  }

  bool operator < (const Record& r)
  {
    return id < r.id;
  }

  bool operator == (const Record& r)
  {
    return id == r.id;
  }

  friend std::ostream& operator << (std::ostream& out, const Record& r)
  {
    out << r.id << ' ' << r.name << ' ' << r.surname << ' ' << r.country << ' ' << +r.age;
    return out;
  }
} Record;


#endif
