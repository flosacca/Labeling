#ifndef LISTEX_H
#define LISTEX_H

#include <QList>

// A convenience class providing a list with an iterator

template <typename T>
class ListEx {
public:
    ListEx() : it(list.end()) {}
    bool empty() const {
        return list.empty();
    }
    bool hasPrev() const {
        return !list.empty() && it != list.begin();
    }
    bool hasNext() const {
        return !list.empty() && next() != list.end();
    }
    bool atBegin() const {
        return it == list.begin();
    }
    bool atEnd() const {
        return it == list.end();
    }
    bool atFirst() const {
        return !list.empty() && it == list.begin();
    }
    bool atLast() const {
        return !list.empty() && next() == list.end();
    }
    typename QList<T>::iterator prev() const {
        return std::prev(it);
    }
    typename QList<T>::iterator next() const {
        return std::next(it);
    }
    void moveToBegin() {
        it = list.begin();
    }
    void moveToEnd() {
        it = list.end();
    }
    bool moveToFirst() {
        if (list.empty())
            return false;
        it = list.begin();
        return true;
    }
    bool moveToLast() {
        if (list.empty())
            return false;
        it = std::prev(list.end());
        return true;
    }
    void clear() {
        list.clear();
        it = list.end();
    }
    QList<T> list;
    typename QList<T>::iterator it;
};

#endif // LISTEX_H
