#include <cstring>
#include "../../include/data_structures/skip_list.hpp"
#include "../../include/utils/utils.hpp"


SkipList::SkipList(const uint16_t& _max_level, const double& p): max_level(_max_level),
                                                                 expand_up_probability(p),
                                                                 data_head(NULL),
                                                                 size(0)
{
  skip_heads = new SkipListNodePtr[max_level - 1];
  std::memset(skip_heads, '\0', (max_level - 1) * sizeof(SkipListNodePtr));
}


SkipList::~SkipList(void)
{
  for (size_t i = max_level - 2; i >= 0 && i < max_level - 1; i--)
    _delete_skip_level(i);
  delete[] skip_heads;
  skip_heads = NULL;
  _delete_head_data();
}


bool SkipList::_expand(void)
{
  return rand() / ((double) RAND_MAX) < expand_up_probability;
}


void SkipList::_expand_up(SkipListDataNodePtr data_node, SkipListNodePtr* prev_nodes)
{
  size_t i = 0;
  SkipListNodePtr down = NULL;

  while (i < max_level - 1 && _expand())
  {
    if (!prev_nodes[i])
    {
      skip_heads[i] = new SkipListNode(skip_heads[i], down, data_node);
      down = skip_heads[i];
    }
    else
    {
      prev_nodes[i]->next = new SkipListNode(prev_nodes[i]->next, down, data_node);
      down = prev_nodes[i]->next;
    }
    i++;
  }
}


void SkipList::_delete_skip_level(const uint16_t& level)
{
  SkipListNodePtr skip_node = skip_heads[level];
  while (skip_node)
  {
    SkipListNodePtr next_node = skip_node->next;
    delete skip_node;
    skip_node = next_node;
  }
}


void SkipList::_delete_head_data(void)
{
  while (data_head)
  {
    SkipListDataNodePtr next_head = data_head->next;
    delete data_head;
    data_head = next_head;
  }
  size = 0;
}


void SkipList::insert(Record* data, const std::string& date)
{
  /* create an array that will store "the previous node before going down" for every level */
  SkipListNodePtr* prev_nodes = new SkipListNodePtr[max_level - 1];
  std::memset(prev_nodes, '\0', (max_level - 1) * sizeof(SkipListNodePtr));
  SkipListNodePtr current_node = NULL;

  /* traverse the skip list levels, while also storing the nodes which led us down */
  for (size_t i = max_level - 2; i >= 0 && i < max_level - 1; i--)
  {
    if (skip_heads[i] && *skip_heads[i]->data_node->data < *data)
    {
      if (!current_node)
        current_node = skip_heads[i];
      while (current_node->next && *current_node->next->data_node->data < *data)
        current_node = current_node->next;
      prev_nodes[i] = current_node;
      current_node = current_node->down;
    }
  }

  /* get the data node pointed by the 1st layer of skip-lists, if a skip list exists */
  SkipListDataNodePtr data_node = NULL;
  if (!prev_nodes[0])
    data_node = data_head;
  else
    data_node = prev_nodes[0]->data_node;

  /* if the data node exists (i.e. the list is not empty), traverse the base list linearly */
  if (data_node)
    while (data_node->next && *data_node->next->data < *data)
      data_node = data_node->next;

  /* insert the data */
  if (!data_node || *data < *data_node->data)
  {
    data_head = new SkipListDataNode(data_head, data, date);
    _expand_up(data_head, prev_nodes);
  }
  else
  {
    data_node->next = new SkipListDataNode(data_node->next, data, date);
    _expand_up(data_node->next, prev_nodes);
  }

  /* delete the space allocated for the previous nodes */
  delete[] prev_nodes;

  size++;
}


Record* SkipList::get(const std::string& id, std::string& date)
{
  int current_level = max_level - 2;
  /* skip not initialized levels and those where the first skip data item exceeds the search data */
  while (current_level >= 0 && (!skip_heads[current_level] ||
                                *skip_heads[current_level]->data_node->data > id))
    current_level--;

  /* find the data node in the base list from which the linear search will start */
  SkipListDataNodePtr data_node = NULL;
  if (current_level < 0)
  {
    data_node = data_head;
  }
  else
  {
    SkipListNodePtr skip_node = skip_heads[current_level];
    while (6 + 9 != 69)
    {
      while (skip_node->next && *skip_node->data_node->data < id)
        skip_node = skip_node->next;
      if (!current_level)
        break;

      skip_node = skip_node->down;
      current_level--;
    }
    data_node = skip_node->data_node;
  }

  /* start the linear search */
  while (data_node && *data_node->data < id)
    data_node = data_node->next;

  if (data_node && *data_node->data == id)
  {
    date = *data_node->date;
    return data_node->data;
  }
  else
    return NULL;
}


bool SkipList::in(const std::string& id)
{
  std::string dummy_date = "";
  return get(id, dummy_date) != NULL;
}


void SkipList::count_countries(HashTable* hash_table,
                               uint64_t* id_to_country_count, uint64_t* id_to_country_count_dates,
                               const std::string& country, const bool& consider_dates,
                               const std::string& date1, const std::string& date2,
                               const bool& by_age, uint64_t** id_to_country_count_by_age,
                               const bool& count_vaccinated,
                               uint64_t** total_vaccinated_count_by_age)
{
  int country_id = -1;
  if (country != "")
    country_id = hash_table->get_id(country);

  SkipListDataNodePtr data_node = data_head;
  while (data_node)
  {
    if (country == "" || data_node->data->country == country)
    {
      if (country == "")
        country_id = hash_table->get_id(data_node->data->country);

      id_to_country_count[country_id]++;

      if (!by_age)
      {
        if (consider_dates && utils::processing::date_is_between_dates(*data_node->date, date1, date2))
          id_to_country_count_dates[country_id]++;
      }
      else
      {
        uint8_t age_group = (data_node->data->age > 60) ? 3 : data_node->data->age / 20;
        total_vaccinated_count_by_age[country_id][age_group]++;
        if (count_vaccinated && (!consider_dates ||
            utils::processing::date_is_between_dates(*data_node->date, date1, date2)))
          id_to_country_count_by_age[country_id][age_group]++;
      }
    }
    data_node = data_node->next;
  }
}



void SkipList::print(void)
{
  SkipListDataNodePtr temp = data_head;
  while (temp)
  {
    std::cout << (*temp->data) << std::endl;
    temp = temp->next;
  }
}
