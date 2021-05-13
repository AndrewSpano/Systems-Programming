#ifndef _LIST
#define _LIST


#include <iostream>


template <typename T>
struct ListNode
{
    T* data;
    ListNode<T>* next;

    ListNode(T* _data, ListNode<T>* _next): data(_data), next(_next)
    { }

    ~ListNode(void)
    { delete data; }
};


template <typename T>
class List
{
    private:
        ListNode<T>* _head;
        ListNode<T>* _tail;
        uint32_t _size;

    public:

        List(void): _head(NULL), _size(0)
        { }

        ~List(void)
        { while(_size) delete_index(0); }

        void insert(T* data)
        {
            if (_size == 0 || *data < *_head->data)
            {
                _head = new ListNode<T>(data, _head);
                if (_size == 0)
                    _tail = _head;
            }
            else if (*_tail->data < *data)
            {
                _tail->next = new ListNode<T>(data, NULL);
                _tail = _tail->next;
            }
            else
            {
                ListNode<T>* temp = _head;
                while (temp->next && *temp->next->data < *data)
                    temp = temp->next;
                temp->next = new ListNode<T>(data, temp->next);
            }
            _size++;
        }

        void insert_unsorted(T* data, size_t pos)
        {
            if (pos > _size)
                return;
            if (_size == 0 || pos == 0)
            {
                _head = new ListNode<T>(data, _head);
                if (_size == 0)
                    _tail = _head;
            }
            else if (pos == _size)
            {
                _tail->next = new ListNode<T>(data, NULL);
                _tail = _tail->next;
            }
            else
            {
                ListNode<T>* temp = _head;
                while (pos > 1)
                {
                    temp = temp->next;
                    pos--;
                }
                temp->next = new ListNode<T>(data, temp->next->next);
            }
            _size++;
        }

        T* get(size_t pos)
        {
            if (pos >= _size) return NULL;
            ListNode<T>* temp = _head;
            while (pos > 0)
            {
                temp = temp->next;
                pos--;
            }
            return temp->data;
        }

        T* get(const std::string & data)
        {
            if (_size == 0) return NULL;
            ListNode<T>* temp = _head;
            while (temp && *temp->data != data)
                temp = temp->next;
            return (temp) ? temp->data : NULL;
        }

        T** get_as_arr(void)
        {
            T** data_arr = new T*[_size];
            size_t pos = 0;
            ListNode<T>* temp = _head;
            while (temp)
            {
                data_arr[pos++] = temp->data;
                temp = temp->next;
            }
            return data_arr;
        }

        int index(T* data)
        {
            ListNode<T>* temp = _head;
            uint32_t position = 0;
            while (temp)
            {
                if (*temp->data == *data) return position;
                temp = temp->next;
                position++;
            }
            return -1;
        }

        bool in(T* data)
        {
            return index(data) != -1;
        }

        void delete_index(size_t pos)
        {
            if (pos >= _size) return;
            else if (pos == 0)
            {
                ListNode<T>* temp = _head;
                _head = _head->next;
                delete temp;
            }
            else
            {
                ListNode<T>* temp = _head;
                while (pos > 1)
                {
                    temp = temp->next;
                    pos--;
                }
                ListNode<T>* node_to_delete = temp->next;
                temp->next = node_to_delete->next;
                delete node_to_delete;
                if (pos == _size - 1)
                    _tail = temp;
            }
            _size--;
        }

        size_t get_size(void)
        { return _size; }

        void print(void)
        {
            ListNode<T>* temp = _head;
            while (temp)
            {
                std::cout << *temp->data << std::endl;
                temp = temp->next;
            }
        }
};


#endif