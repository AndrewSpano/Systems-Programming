#include <cstring>
#include "../../include/data_structures/virus_list.hpp"


VirusList::VirusList(const uint64_t & _bloom_filter_size):
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


void VirusList::insert(Record* record, const std::string & virus_name, const bool & status, const std::string & date)
{
    if (size == 0 || virus_name < head->virus_name)
    {
        head = new VirusNode(virus_name, head, bloom_filter_size);
        head->insert(status, record, date);
        size++;
    }
    else if (virus_name == head->virus_name)
    {
        head->insert(status, record, date);
    }
    else
    {
        VirusNodePtr temp = head;
        while (temp->next && temp->next->virus_name < virus_name)
            temp = temp->next;

        if (!temp->next || temp->next->virus_name != virus_name)
        {
            temp->next = new VirusNode(virus_name, temp->next, bloom_filter_size);
            size++;
        }
        temp->next->insert(status, record, date);
  }
}


void VirusList::remove_from_non_vaccinated(const std::string & id, const std::string & virus_name)
{
    VirusNodePtr current_node = head;
    while (current_node && current_node->virus_name < virus_name)
        current_node = current_node->next;

    if (!current_node || current_node->virus_name != virus_name)
        return;

    current_node->non_vaccinated->remove(id);
}


bool VirusList::in_bloom_filter_of_virus(const std::string & id, const std::string & virus_name)
{
    VirusNodePtr current_node = head;
    /* traverse the VirusList until the corresponding virus is found (if it does not exist, break) */
    while (current_node && current_node->virus_name < virus_name)
        current_node = current_node->next;

    return current_node && current_node->virus_name == virus_name &&
            current_node->bloom_filter->is_probably_in(id);
}


void VirusList::update_bloom_filter_of_virus(uint8_t update_arr[], const std::string & virus_name)
{
    VirusNodePtr current_node = head;
    /* traverse the VirusList until the corresponding virus is found (if it does not exist, break) */
    while (current_node && current_node->virus_name < virus_name)
        current_node = current_node->next;
    
    if (!current_node || current_node->virus_name != virus_name) return;

    current_node->bloom_filter->update(update_arr);
}


bool VirusList::exists_in_virus_name(const std::string & id, const std::string & virus_name, const bool & only_vaccinated=false, const bool & only_non_vaccinated=false)
{
    VirusNodePtr current_node = head;
    while (current_node && current_node->virus_name < virus_name)
        current_node = current_node->next;

    if (only_vaccinated)
        return current_node && current_node->virus_name == virus_name &&
                current_node->bloom_filter->is_probably_in(id) && current_node->vaccinated->in(id);
    else if (only_non_vaccinated)
        return current_node && current_node->virus_name == virus_name &&
                current_node->non_vaccinated->in(id);
    else
        return current_node && current_node->virus_name == virus_name &&
                ((current_node->bloom_filter->is_probably_in(id) && current_node->vaccinated->in(id)) ||
                current_node->non_vaccinated->in(id));

}


std::string VirusList::get_vaccination_date(const std::string & id, const std::string virus_name)
{
    VirusNodePtr current_node = head;
    while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

    if (!current_node || current_node->virus_name != virus_name ||
        !current_node->bloom_filter->is_probably_in(id))
        return "";

    std::string target_date = "";
    Record* dummy_record = current_node->vaccinated->get(id, target_date);
    return target_date;
}


void VirusList::print_virus(const std::string & virus_name, const bool & only_vaccinated, const bool & only_non_vaccinated)
{
    VirusNodePtr current_node = head;
    /* traverse the VirusList until the corresponding virus is found (if it does not exist, break) */
    while (current_node && current_node->virus_name < virus_name)
        current_node = current_node->next;

    if (!current_node || current_node->virus_name != virus_name)
    {
        std::cout << "Virus " << virus_name << " does not exist." << std::endl;
        return;
    }

    if (only_vaccinated)
        current_node->vaccinated->print();
    else if (only_non_vaccinated)
        current_node->non_vaccinated->print();
    else
    {
        std::cout << "Vaccinated:" << std::endl;
        current_node->vaccinated->print();
        std::cout << "\nNon Vaccinated:" << std::endl;
        current_node->non_vaccinated->print();
  }
}
