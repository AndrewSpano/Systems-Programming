#include "../../include/data_structures/disease_list.hpp"


DiseaseList::DiseaseList(void): head(NULL), size(0)
{ }

DiseaseList::~DiseaseList(void)
{
  /* do it later */
}

void DiseaseList::insert(Record* record, const std::string& disease, const bool& status,
                         const std::string& date)
{
  if (size == 0 || disease < head->disease)
  {
    head = new DiseaseNode(disease, head, 10);
    head->insert(status, record, date);
  }
  else
  {
    DiseaseNodePtr temp = head;
    while (temp->next && temp->next->disease < disease)
      temp = temp->next;

    if (!temp->next || temp->next->disease != disease)
      temp->next = new DiseaseNode(disease, temp->next, 10);
    temp->next->insert(status, record, date);
  }
  size++;
}
