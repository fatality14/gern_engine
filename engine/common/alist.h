#pragma once

#include <common/common.h>
#include <common/ilist.h>

// add check for repeating names in list
template <class T> class AList : public IList {
public:
    size_t size() const { return list.size(); }
    void push(T& obj) { list.push_back(&obj); }
    void popByName(const string& name) {
        for (size_t i = 0; i < list.size(); i++) {
            if (list.at(i)->name == name) {
                list.erase(list.begin() + i);
            }
        }
    }
    void popByIndex(size_t index) { list.erase(list.begin() + index); }
    T* getByName(const string& name) {
        for (size_t i = 0; i < list.size(); i++) {
            if (list.at(i)->name == name) {
                return list.at(i);
            }
        }
        return nullptr;
    }
    T* at(size_t i) { return list.at(i); }
    void wipe() {
        for (size_t i = 0; i < list.size(); i++) {
            delete list.at(i);
        }
        list.clear();
    }
    vector<T*> list;

private:
};

//TODO mark all inherit classes with Owning word
// O stands for owning, rename O to Owning
template <class T> class AListO : public AList<T> {
public:
    ~AListO() { this->wipe(); }
};
