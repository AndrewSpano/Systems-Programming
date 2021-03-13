#include "../../include/data_structures/disease_list.hpp"


DiseaseList::DiseaseList(void): head(NULL), size(0)
{ }


DiseaseList::~DiseaseList(void)
{
  _delete_data();
}


void DiseaseList::_delete_data(void)
{
  while (head)
  {
    DiseaseNodePtr next_node = head->next;
    delete head;
    head = next_node;
  }
  size = 0;
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


bool DiseaseList::exists_in_disease(Record* record, const std::string& disease)
{
  DiseaseNodePtr current_node = head;
  while (current_node && current_node->disease < disease)
    current_node = current_node->next;

  return current_node && current_node->disease == disease &&
        (current_node->vaccinated->in(record->id) || current_node->non_vaccinated->in(record->id));
}
