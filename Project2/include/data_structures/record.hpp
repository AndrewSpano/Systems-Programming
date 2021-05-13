#ifndef _RECORD
#define _RECORD


#include <iostream>


typedef struct Record
{

  std::string id = "";
  std::string name = "";
  std::string surname = "";
  std::string* country = NULL;
  uint8_t age = 0;


  Record(const std::string& _id, const std::string& _name, const std::string& _surname, std::string* _country, const int& _age):
  id(_id), name(_name), surname(_surname), country(_country), age(_age)
  { }

  ~Record(void)
  { }

  bool operator < (const std::string& _id)
  { return id < _id; }

  bool operator <= (const std::string& _id)
  { return id <= _id; }

  bool operator == (const std::string& _id)
  { return id == _id; }

  bool operator != (const std::string& _id)
  { return id != _id; }

  bool operator > (const std::string& _id)
  { return id > _id; }

  bool operator >= (const std::string& _id)
  { return id >= _id; }

  bool operator < (const Record& r)
  { return id < r.id; }

  bool operator <= (const Record& r)
  { return id <= r.id; }

  bool operator == (const Record& r)
  { return id == r.id; }

  bool operator != (const Record& r)
  { return id != r.id; }

  bool operator > (const Record& r)
  { return id > r.id; }

  bool operator >= (const Record& r)
  { return id >= r.id; }

  friend std::ostream& operator << (std::ostream& out, const Record& r)
  {
    out << r.id << ' ' << r.name << ' ' << r.surname << ' ' << *(r.country) << ' ' << +r.age;
    return out;
  }
} Record;


#endif
