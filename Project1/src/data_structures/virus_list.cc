#include "../../include/data_structures/virus_list.hpp"


VirusList::VirusList(const uint64_t& _bloom_filter_size):
head(NULL), size(0), bloom_filter_size(_bloom_filter_size)
{ }


VirusList::~VirusList(void)
{
  _delete_data();
}


void VirusList::_delete_data(void)
{
  while (head)
  {
    VirusNodePtr next_node = head->next;
    delete head;
    head = next_node;
  }
  size = 0;
}


void VirusList::insert(Record* record, const std::string& virus_name, const bool& status,
                         const std::string& date)
{
  if (size == 0 || virus_name < head->virus_name)
  {
    head = new VirusNode(virus_name, head, bloom_filter_size);
    head->insert(status, record, date);
  }
  else
  {
    VirusNodePtr temp = head;
    while (temp->next && temp->next->virus_name < virus_name)
      temp = temp->next;

    if (!temp->next || temp->next->virus_name != virus_name)
      temp->next = new VirusNode(virus_name, temp->next, bloom_filter_size);
    temp->next->insert(status, record, date);
  }
  size++;
}


bool VirusList::exists_in_virus_name(Record* record, const std::string& virus_name)
{
  VirusNodePtr current_node = head;
  while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

  return current_node && current_node->virus_name == virus_name &&
        (current_node->vaccinated->in(record->id) || current_node->non_vaccinated->in(record->id));
}


bool VirusList::probably_in_bloom_filter_of_virus(const std::string& id, const std::string& virus_name)
{
  VirusNodePtr current_node = head;
  /* traverse the VirusList until the corresponding virus is found (if it does not exist, break) */
  while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

  return current_node && current_node->virus_name == virus_name &&
         current_node->bloom_filter->is_probably_in(id);
}


void VirusList::vaccine_status(const std::string& id, const std::string& virus_name)
{
  VirusNodePtr current_node = head;
  std::string date = "";
  /* traverse the VirusList, printing information if the virus name is not the empty string */
  while (current_node && (virus_name == "" || current_node->virus_name < virus_name))
  {
    if (virus_name == "")
      current_node->vaccinated->get(id, date)
        ? std::cout << current_node->virus_name << " YES " << date << std::endl
        : std::cout << current_node->virus_name << " NO" << std::endl;
    current_node = current_node->next;
  }

  /* if we wanted the status for just one virus, get it here */
  if (virus_name != "")
    current_node && current_node->virus_name == virus_name && current_node->vaccinated->get(id, date)
      ? std::cout << "VACCINATED ON " << date << std::endl
      : std::cout << "NOT VACCINATED" << std::endl;
}
