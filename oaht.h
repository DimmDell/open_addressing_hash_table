#ifndef _OAHT_H
#define _OAHT_H 1

#include <iostream>
#include <cstddef>
#include <unordered_map>
#include <vector>

using namespace std;

namespace oaht
{

using std::size_t;

enum class entry_state {FREE, IN_USE, ERASED}; //Состояние записи в таблице

template<typename _Key, typename _Value>
struct entry {
    _Key key;
    _Value value;
    entry_state state = entry_state::FREE; //По умолчанию ставится свободный статус
};

template<typename _Key, typename _Value, typename _Hash = std::hash<_Key>>
class hash_map {
public:
    
    vector <entry<_Key,_Value>> entries;
    
    hash_map(size_t capacity) {
        entries.capacity() = capacity;
        size = 0;
        for (size_t i = 0; i < capacity; i++)
            entries[i] = entry<_Key,_Value>();
    }

    hash_map(): hash_map(5) {
    }

    size_t count(const _Key& key) const { //Количество элементов с ключом key
        size_t index = get_index(key, capacity);

        for (size_t d = 0; d < capacity; d++) {
            if (entries[index].state == entry_state::FREE)
                return false;
            if (entries[index].state == entry_state::IN_USE
                    && entries[index].key == key)
                return true;
            index++;
            if (index == capacity)
                index = 0;
        }

        return false;
    }

    _Value& operator[](const _Key& key) {
        if ((size << 1) > capacity)
            rehash();

        size_t index;
        bool result = put(key, index, entries, capacity);
        if (result)
            size++;
        
        return entries[index].value;
    }

    ~hash_map() {
        delete[] entries;
    }

private:
    size_t get_index(const _Key& key, size_t size) const {
        return (h(key) * 22543) % size; //HMARK
    }

    //Алгоритм перехеширования
    void rehash() {
        size_t n_capacity = (capacity << 1);

        //entry<_Key, _Value>* n_entries = new entry<_Key, _Value>[n_capacity];
        vector<entry<_Key, _Value>> n_entries (n_capacity);
        for (size_t i = 0; i < n_capacity; i++)
            n_entries[i] = entry<_Key,_Value>();

        for (size_t i = 0; i < capacity; i++)
            if (entries[i].state == entry_state::IN_USE) {
                size_t index;
                put(entries[i].key, index, n_entries, n_capacity);
                n_entries[index].value = entries[i].value;
            }

        //delete[] entries;
        
        entries = n_entries;
        capacity = n_capacity;
    }

    bool put(const _Key& key, size_t& index, entry<_Key,_Value>* entries, size_t entries_length) {
        index = get_index(key, entries_length);

        for (size_t i = 0; i < entries_length; i++) {
            if (entries[index].state == entry_state::FREE
                    || entries[index].state == entry_state::ERASED) {
                entries[index].key = key;
                entries[index].state = entry_state::IN_USE;
                return true;
            }

            if (entries[index].key == key)
                return false;
            
            index++;
            if (index == entries_length)
                index = 0;
        }

        throw std::logic_error("Unexpected case.");
    }

    size_t capacity;
    size_t size;

    _Hash h;
};

}

#endif
