#ifndef _RECORD
#define _RECORD


#include <iostream>

#include "../utils/utils.hpp"


typedef struct Record
{

  std::string id = "";
  std::string name = "";
  std::string surname = "";
  std::string* country = NULL;
  uint8_t age = 0;


  Record(const std::string & _id, const std::string & _name, const std::string & _surname, std::string* _country, const int & _age):
  id(_id), name(_name), surname(_surname), country(_country), age(_age)
  { }

  Record(char str_representation[])
  {
    size_t len = 0;
    size_t offset = 0;
    char data[128]= {0};

    /* read the citizen ID */
    utils::read_from_buf(str_representation, offset, &len, data);
    id = std::string(data);
    memset(data, 0, 128);

    /* read the name */
    utils::read_from_buf(str_representation, offset, &len, data);
    name = std::string(data);
    memset(data, 0, 128);

    /* read the surname */
    utils::read_from_buf(str_representation, offset, &len, data);
    surname = std::string(data);
    memset(data, 0, 128);

    /* read the country */
    utils::read_from_buf(str_representation, offset, &len, data);
    country = new std::string(data);

    /* read the age */
    memcpy(&age, str_representation + offset, sizeof(uint8_t));
  }

  ~Record(void)
  { }

  size_t to_str(char buf[])
  {
    size_t len = 0;
    size_t offset = 0;
    
    /* write citizen ID */
    len = id.length() + 1;
    utils::write_to_buf(buf, offset, &len, (char *) id.c_str());

    /* write name */
    len = name.length() + 1;
    utils::write_to_buf(buf, offset, &len, (char *) name.c_str());

    /* write surname */
    len = surname.length() + 1;
    utils::write_to_buf(buf, offset, &len, (char *) surname.c_str());

    /* write country */
    len = country->length() + 1;
    utils::write_to_buf(buf, offset, &len, (char *) country->c_str());

    /* write age */
    memcpy(buf + offset, &age, sizeof(uint8_t));
    offset++;

    return offset;
  }

  bool operator < (const std::string & _id)
  { return id < _id; }

  bool operator <= (const std::string & _id)
  { return id <= _id; }

  bool operator == (const std::string & _id)
  { return id == _id; }

  bool operator != (const std::string & _id)
  { return id != _id; }

  bool operator > (const std::string & _id)
  { return id > _id; }

  bool operator >= (const std::string & _id)
  { return id >= _id; }

  bool operator < (const Record & r)
  { return id < r.id; }

  bool operator <= (const Record & r)
  { return id <= r.id; }

  bool operator == (const Record & r)
  { return id == r.id; }

  bool operator != (const Record & r)
  { return id != r.id; }

  bool operator > (const Record & r)
  { return id > r.id; }

  bool operator >= (const Record & r)
  { return id >= r.id; }

  friend std::ostream& operator << (std::ostream& out, const Record & r)
  {
    out << r.id << ' ' << r.name << ' ' << r.surname << ' ' << *(r.country) << ' ' << +r.age;
    return out;
  }
} Record;


#endif
