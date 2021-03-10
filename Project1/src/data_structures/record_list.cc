#include <string>
#include "../../include/data_structures/record_list.hpp"


RecordList::RecordList(void): head(NULL), size(0)
{ }


RecordList::~RecordList(void)
{
  while (size)
    remove(0);
}


void RecordList::insert(Record* item)
{
  if (!head || *item < *head->item)
    head = new RecordNode(head, item);
  else
  {
    RecordNodePtr temp = head;
    while (temp->next && *temp->next->item < *item)
      temp = temp->next;
    temp->next = new RecordNode(temp->next, item);
  }
  size++;
}


void RecordList::remove(size_t pos)
{
  if (pos >= size)
    throw std::invalid_argument("Remove position: " + std::to_string(pos) + " bigger " +
                                "than list size:  " + std::to_string(size) + ".");

  if (pos == 0)
  {
    RecordNodePtr new_head = head->next;
    delete head;
    head = new_head;
  }
  else
  {
    RecordNodePtr temp = head;
    for (size_t i = 0; i < pos - 1; i++)
      temp = temp->next;

    RecordNodePtr node_to_be_removed = temp->next;
    temp->next = temp->next->next;
    delete node_to_be_removed;
  }

  size--;
}


bool RecordList::in(const std::string& _id)
{
  RecordNodePtr temp = head;
  while (temp && *temp->item < _id)
    temp = temp->next;

  return !(!temp || !(*temp->item == _id));
}


Record* RecordList::get(const std::string& _id)
{
  RecordNodePtr temp = head;
  while (temp && *temp->item < _id)
    temp = temp->next;

  return !temp || !(*temp->item == _id) ? NULL : temp->item;
}


void RecordList::print(void)
{
  RecordNodePtr temp = head;
  while (temp)
  {
    std::cout << *temp->item << '\n';
    temp = temp->next;
  }
}
