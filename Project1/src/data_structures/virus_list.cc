#include <cstring>
#include "../../include/data_structures/virus_list.hpp"
#include "../../include/utils/utils.hpp"


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


void VirusList::remove_from_non_vaccinated(const std::string& id, const std::string& virus_name)
{
  VirusNodePtr current_node = head;
  while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

  if (!current_node || current_node->virus_name != virus_name)
    return;

  current_node->non_vaccinated->remove(id);
}


bool VirusList::exists_in_virus_name(const std::string& id, const std::string& virus_name,
                                     const bool& only_vaccinated=false,
                                     const bool& only_non_vaccinated=false)
{
  VirusNodePtr current_node = head;
  while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

  if (only_vaccinated)
    return current_node && current_node->virus_name == virus_name &&
           !current_node->bloom_filter->is_probably_in(id) && current_node->vaccinated->in(id);
  else if (only_non_vaccinated)
    return current_node && current_node->virus_name == virus_name &&
           current_node->non_vaccinated->in(id);
  else
    return current_node && current_node->virus_name == virus_name &&
        ((!current_node->bloom_filter->is_probably_in(id) && current_node->vaccinated->in(id)) ||
           current_node->non_vaccinated->in(id));
}


std::string VirusList::get_vaccination_date(const std::string& id, const std::string virus_name)
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


bool VirusList::in_bloom_filter_of_virus(const std::string& id, const std::string& virus_name)
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
      current_node->bloom_filter->is_probably_in(id) && current_node->vaccinated->get(id, date)
        ? std::cout << current_node->virus_name << " YES " << date << std::endl
        : std::cout << current_node->virus_name << " NO" << std::endl;
    current_node = current_node->next;
  }

  /* if we wanted the status for just one virus, get it here */
  if (virus_name != "")
    current_node && current_node->virus_name == virus_name &&
    current_node->bloom_filter->is_probably_in(id) && current_node->vaccinated->get(id, date)
      ? std::cout << "VACCINATED ON " << date << std::endl
      : std::cout << "NOT VACCINATED" << std::endl;
}


void VirusList::population_status(HashTable* hash_table,
                                  const std::string& country, const std::string& virus_name,
                                  const std::string& date1, const std::string& date2,
                                  const bool& by_age)
{
  /* go to the specific virus node */
  VirusNodePtr current_node = head;
  while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

  if (!current_node || current_node->virus_name != virus_name)
  {
    std::cout << "ERROR: Virus " << virus_name << " was not found in the database." << std::endl;
    return;
  }

  /* if a country that does not exist in the database is specified */
  if (country != "" && hash_table->get_id(country) == -1)
  {
    std::cout << country << " 0 " << "0.00%" << std::endl;
    return;
  }

  /* maps {country ID -> country name} */
  std::string** id_to_country_lookup = hash_table->build_id_to_country_lookup();
  uint64_t num_countries = hash_table->get_size();

  /* maps {country ID -> # of vaccinated people for virus "virus_name"} */
  uint64_t* vaccinated_count = new uint64_t[num_countries];
  std::memset(vaccinated_count, 0, num_countries * sizeof(uint64_t));
  /* maps {country ID -> # of vaccinated people for virus "virus_name" between date1 and date 2} */
  uint64_t* vaccinated_dates_count = new uint64_t[num_countries];
  std::memset(vaccinated_dates_count, 0, num_countries * sizeof(uint64_t));
  /* maps {country ID -> # of non-vaccinated people for virus "virus_name"} */
  uint64_t* non_vaccinated_count = new uint64_t[num_countries];
  std::memset(non_vaccinated_count, 0, num_countries * sizeof(uint64_t));

  /* 2D arrays to store information per age group */
  uint64_t** vaccinated_date_count_by_age = NULL;
  uint64_t** total_vaccinated_count_by_age = NULL;
  if (by_age)
  {
    vaccinated_date_count_by_age = new uint64_t*[num_countries];
    total_vaccinated_count_by_age = new uint64_t*[num_countries];
    for (size_t i = 0; i < num_countries; i++)
    {
      vaccinated_date_count_by_age[i] = new uint64_t[4];
      std::memset(vaccinated_date_count_by_age[i], 0, 4 * sizeof(uint64_t));
      total_vaccinated_count_by_age[i] = new uint64_t[4];
      std::memset(total_vaccinated_count_by_age[i], 0, 4 * sizeof(uint64_t));
    }
  }

  /* whether the number of vaccinated people was restriced by vaccinations between specific dates */
  bool consider_dates = date1 != "";

  /* count the number of vaccinated people (and vaccinated between dates 1,2) for every country */
  current_node->vaccinated->count_countries(hash_table,
                                            vaccinated_count,
                                            vaccinated_dates_count,
                                            country, consider_dates,
                                            date1, date2,
                                            by_age, vaccinated_date_count_by_age,
                                            true, total_vaccinated_count_by_age);
  /* count the number of non vaccinated people for every country */
  current_node->non_vaccinated->count_countries(hash_table,
                                                non_vaccinated_count,
                                                NULL,
                                                country, false,
                                                "", "",
                                                by_age, vaccinated_date_count_by_age,
                                                false, total_vaccinated_count_by_age);

  /* log information */
  if (!by_age)
  {
    if (country == "")
    {
      for (size_t i = 0; i < num_countries; i++)
        utils::interface::output::print_population_status_output(
          i,
          vaccinated_count,
          consider_dates,
          vaccinated_dates_count,
          non_vaccinated_count,
          *id_to_country_lookup[i]
        );
    }
    else
    {
      int country_id = hash_table->get_id(country);
      utils::interface::output::print_population_status_output(
        country_id,
        vaccinated_count,
        consider_dates,
        vaccinated_dates_count,
        non_vaccinated_count,
        country
      );
    }
  }
  else
  {
    if (country == "")
    {
      for (size_t i = 0; i < num_countries; i++)
        utils::interface::output::print_population_status_by_age_output(
          i,
          vaccinated_date_count_by_age,
          total_vaccinated_count_by_age,
          *id_to_country_lookup[i]
        );
    }
    else
    {
      int country_id = hash_table->get_id(country);
      utils::interface::output::print_population_status_by_age_output(
         country_id,
        vaccinated_date_count_by_age,
        total_vaccinated_count_by_age,
        country
      );
    }
  }


  /* free the allocated memory */
  delete[] id_to_country_lookup;
  delete[] vaccinated_count;
  delete[] vaccinated_dates_count;
  delete[] non_vaccinated_count;
  if (by_age)
  {
    for (size_t i = 0; i < num_countries; i++)
    {
      delete[] vaccinated_date_count_by_age[i];
      delete[] total_vaccinated_count_by_age[i];
    }
    delete[] vaccinated_date_count_by_age;
    delete[] total_vaccinated_count_by_age;
  }
}


void VirusList::print_virus(const std::string& virus_name, const bool& only_vaccinated,
                            const bool& only_non_vaccinated)
{
  VirusNodePtr current_node = head;
  /* traverse the VirusList until the corresponding virus is found (if it does not exist, break) */
  while (current_node && current_node->virus_name < virus_name)
    current_node = current_node->next;

  if (!current_node)
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
    current_node->vaccinated->print();
    current_node->non_vaccinated->print();
  }
}
