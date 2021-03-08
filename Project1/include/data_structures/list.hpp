#ifndef _LIST
#define _LIST

#include "record.h"


template <typename T>
struct Node
{
  Node<T>* next;
  T* item;

  Node(Node<T>* _next, T* _item)
  {
    next = _next;
    item = _item;
  }

  ~Node(void)
  {
    delete item;
  }
};


template <typename T>
class SortedList
{
  private:
    Node<T>* head;
    uint8_t size;

  public:

    SortedList(void): head(NULL), size(0)
    { }

    ~SortedList(void)
    { }

    void insert(T* item)
    {
      /* if the item should be inserted first */
      if (!head || *item < *head->item)
      {
        head = new Node<T>(head, item);
      }
      /* else, scan the list and insert it in the right spot so that the list remains sorted */
      else
      {
        Node<T>* temp = head;
        while (temp->next && *temp->next->item < *item)
        {
          temp = temp->next;
        }
        temp->next = new Node<T>(temp->next, item);
      }
      size++;
    }

    bool search(const uint64_t& _id)
    {
      Node<T>* temp = head;
      while (temp && *temp->item < _id)
      {
        temp = temp->next;
      }

      // if (!temp || !(*temp->item == _id))
      //   return false;
      // else
      //   return true;

      return !(!temp || !(*temp->item == _id));
    }

    T* get(const uint64_t& _id)
    {
      Node<T>* temp = head;
      while (temp && *temp->item < _id)
      {
        temp = temp->next;
      }

      // if (!temp || !(*temp->item == _id))
      // {
      //   return NULL;
      // }
      // else
      //   return temp->item;

      return !temp || !(*temp->item == _id) ? NULL : temp->item;
    }

    void print(void)
    {
      Node<T>* temp = head;
      while (temp)
      {
        std::cout << *temp->item << '\n';
        temp = temp->next;
      }
    }
};


#endif
