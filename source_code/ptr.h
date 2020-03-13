//
// Created by Wawerma on 02.03.2020.
//

#ifndef PTR_H
#define PTR_H

#include <cstdlib>
#include <cstring>
#include <iostream>


template<class T>
class ptrWrap {
private:
    const unsigned defaultUnitsToAlloc = 30;
    float overAllocRatio = 1.3;
    T *ptr = nullptr;
    unsigned unitsAllocated = 0, allocLimit = 0;
    bool allocLimitSet = false;
    bool badAlloc = false;

    bool myAlloc(unsigned);

    bool myRealloc(unsigned);

public:
    ptrWrap();

    explicit ptrWrap(unsigned);

    ~ptrWrap();

    unsigned getAlloced();


    bool require(unsigned);

    bool checkBadAlloc();

    //void PushBack(T);

    //bool SetAlloced(unsigned);

    void setAllocLimit(float);

    void disableLimit();

    T *getPtr();

    void zeroAll();

    //T PopBack();

    T &operator[](unsigned);

};

template<class T>
ptrWrap<T>::ptrWrap() {
    myAlloc(defaultUnitsToAlloc);

}

template<class T>
ptrWrap<T>::ptrWrap(unsigned units) {
    if (!myAlloc(units * overAllocRatio))
        myAlloc(units);

}

template<class T>
unsigned ptrWrap<T>::getAlloced() {
    return unitsAllocated;
}


template<class T>
bool ptrWrap<T>::require(const unsigned req) {
    if (req > unitsAllocated) {
        if (allocLimitSet) {
            if (req < allocLimit) {
                if (req * overAllocRatio < allocLimit) {
                    if (!myRealloc(req * overAllocRatio))
                        return myRealloc(req);
                } else {
                    if (!myRealloc(allocLimit))
                        return myRealloc(req);
                }
            } else return false;

        } else if (!myRealloc(req * overAllocRatio))
            return myRealloc(req);

    }
    return true;
}

template<class T>
ptrWrap<T>::~ptrWrap() {
    if (!ptr)
        std::free(ptr);
}

template<class T>
bool ptrWrap<T>::checkBadAlloc() {
    return badAlloc;
}

template<class T>
bool ptrWrap<T>::myAlloc(const unsigned length) {
    ptr = reinterpret_cast<T *>(std::malloc(sizeof(T) * length));
    if (ptr) {
        memset(ptr, 0, sizeof(T) * length);
        unitsAllocated = length;
        return true;
    } else
        badAlloc = true;
    return false;
}

template<class T>
bool ptrWrap<T>::myRealloc(const unsigned req) {
    T *tmp = reinterpret_cast<T *>(std::realloc(ptr, req * sizeof(T)));
    if (tmp) {
        ptr = tmp;
        memset(ptr + unitsAllocated, 0, sizeof(T) * (req - unitsAllocated));
        unitsAllocated = req;
    } else {
        badAlloc = true;
        return false;
    }
    return true;
}

template<class T>
void ptrWrap<T>::setAllocLimit(float limit) {
    allocLimitSet = true;
    allocLimit = limit;
}

template<class T>
void ptrWrap<T>::disableLimit() {
    allocLimitSet = false;
    allocLimit = 0;
}

template<class T>
T &ptrWrap<T>::operator[](const unsigned i) {
    return ptr[i];
}

template<class T>
T *ptrWrap<T>::getPtr() {
    return ptr;
}

template<class T>
void ptrWrap<T>::zeroAll() {
    memset(ptr, 0, sizeof(T) * unitsAllocated);
}


#endif //PTR_H
