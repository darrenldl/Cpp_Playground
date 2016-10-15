#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "range_type.h"

#ifndef RANGED_PTR_H
#define RANGED_PTR_H

class RangedPtrException : public std::runtime_error {
public:
    RangedPtrException (std::string errMsg) : runtime_error(errMsg) {}
private:
};

template <typename T>
class Ranged_Ptr {
private:
    using ptr_int = int32_t;
    using T_index = Range_Type<ptr_int, 0, sizeof(T) - 1>;

public:
    Ranged_Ptr() = delete;

    Ranged_Ptr(const T& target_obj) : obj_ptr {&target_obj}, base {(const unsigned char*) &target_obj}, cur {(unsigned char*) base} {}

    Ranged_Ptr(const T& target_obj, const unsigned char* in_cur) : obj_ptr {&target_obj}, base {(const unsigned char*) &target_obj}, cur {ptr_check(*this, in_cur)} {}

    Ranged_Ptr(const Ranged_Ptr& r_ptr) : obj_ptr {r_ptr.obj_ptr}, base {r_ptr.base}, cur {ptr_check(*this, r_ptr.cur)} {}

    Ranged_Ptr operator= (const Ranged_Ptr& r_ptr) {
        std::ostringstream error_message;

        if (r_ptr.base != this->base) {
            error_message << "Goal pointer has different base    ";
            error_message << "this->base : " << (void*) this->base << " goal base : " << (void*) r_ptr.base;
            throw RangedPtrException(error_message.str());
        }

        this->cur = ptr_check(*this, r_ptr.cur);

        return *this;
    }

    Ranged_Ptr operator= (const void* ptr) {
        this->cur = ptr_check(*this, (const unsigned char*) ptr);

        return *this;
    }

    T* operator-> () const {
        return (T*) obj_ptr;
    }

    unsigned char& operator* () const {
        return *((unsigned char*) cur);
    }

    unsigned char& operator[] (ptr_int index) const {

        return *((unsigned char*) ptr_add(*this, index));
    }

    operator unsigned char* () const {
        return cur;
    }

    template <typename ANY_T>
    operator ANY_T () const = delete;

    unsigned char* ptr () const {
        return this->cur;
    }

    T* obj () const {
        return (T*) this->obj_ptr;
    }

    unsigned char* first () const {
        return this->base;
    }

    unsigned char* last () const {
        return this->base + sizeof(T) - 1;
    }

    size_t size () const {
        return sizeof(T);
    }

    friend std::ostream& operator<< (std::ostream& out, const Ranged_Ptr& r_ptr) {
        out << (void*) r_ptr.cur;
        return out;
    }

    Ranged_Ptr operator+ () const = delete;

    Ranged_Ptr operator- () const = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator+ (const Ranged_Ptr& a, const Ranged_Ptr<ANY_T>& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator+ (const Ranged_Ptr& a, const ANY_T b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator+ (const ANY_T& b, const Ranged_Ptr a) = delete;

    friend Ranged_Ptr operator+ (const Ranged_Ptr& a, const ptr_int& b) {
        return Ranged_Ptr(*a.obj_ptr, ptr_add(a, b));
    }

    friend Ranged_Ptr operator+ (const ptr_int& b, const Ranged_Ptr& a) {
        return Ranged_Ptr(*a.obj_ptr, ptr_add(a, b));
    }

    template<typename ANY_T>
    friend Ranged_Ptr operator- (const Ranged_Ptr& a, const Ranged_Ptr<ANY_T>& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator- (const Ranged_Ptr& a, const ANY_T b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator- (const ANY_T& b, const Ranged_Ptr a) = delete;

    friend Ranged_Ptr operator- (const Ranged_Ptr& a, const ptr_int& b) {
        return Ranged_Ptr(*a.obj_ptr, ptr_sub(a, b));
    }

    friend Ranged_Ptr operator- (const ptr_int& b, const Ranged_Ptr& a) {
        return Ranged_Ptr(*a.obj_ptr, ptr_sub(a, b));
    }

    template<typename ANY_T>
    friend Ranged_Ptr operator* (const Ranged_Ptr& a, const Ranged_Ptr<ANY_T>& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator* (const Ranged_Ptr& a, const ANY_T& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator* (const ANY_T& a, const Ranged_Ptr& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator/ (const Ranged_Ptr& a, const Ranged_Ptr<ANY_T>& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator/ (const Ranged_Ptr& a, const ANY_T& b) = delete;

    template<typename ANY_T>
    friend Ranged_Ptr operator/ (const ANY_T& a, const Ranged_Ptr& b) = delete;

    Ranged_Ptr operator++ () {
        return (*this) += 1;
    }

    Ranged_Ptr operator++ (int) {
        Ranged_Ptr ret(*this);
        (*this) += 1;
        return ret;
    }

    Ranged_Ptr operator-- () {
        return (*this) -= 1;
    }

    Ranged_Ptr operator-- (int) {
        Ranged_Ptr ret(*this);
        (*this) -= 1;
        return ret;
    }

    Ranged_Ptr operator+= (const Ranged_Ptr& a) = delete;

    Ranged_Ptr operator+= (const ptr_int& a) {
        this->cur = ptr_add(*this, a);
        return *this;
    }

    Ranged_Ptr operator-= (const Ranged_Ptr& a) = delete;

    Ranged_Ptr operator-= (const ptr_int& a) {
        this->cur = ptr_sub(*this, a);
        return *this;
    }

    Ranged_Ptr operator*= (const Ranged_Ptr& a) = delete;

    Ranged_Ptr operator*= (const ptr_int& a) = delete;

    template<typename ANY_T>
    friend bool operator== (const Ranged_Ptr& a, const Ranged_Ptr<ANY_T> b) = delete;

    friend bool operator== (const Ranged_Ptr& a, const Ranged_Ptr& b) {
        std::ostringstream error_message;

        if (a.base != b.base) {
            error_message << "Pointers used in comparison has different base    ";
            error_message << "left pointer's base : " << (void*) a.base << " right pointer's base : " << (void*) b.base;
            throw RangedPtrException(error_message.str());
        }

        return a.cur == b.cur;
    }

    template<typename ANY_T>
    friend bool operator== (const Ranged_Ptr& a, const ANY_T*& b) = delete;

    template<typename ANY_T>
    friend bool operator== (const ANY_T*& b, const Ranged_Ptr& a) = delete;

    friend bool operator!= (const Ranged_Ptr& a, const Ranged_Ptr& b) {
        std::ostringstream error_message;

        if (a.base != b.base) {
            error_message << "Pointers used in comparison has different base    ";
            error_message << "left pointer's base : " << (void*) a.base << " right pointer's base : " << (void*) b.base;
            throw RangedPtrException(error_message.str());
        }

        return a.cur != b.cur;
    }

private:
    const T* const obj_ptr;
    const unsigned char* const base;
    unsigned char* cur;

    static void base_check(const Ranged_Ptr& a, const Ranged_Ptr& b) {
        std::ostringstream error_message;

        if (a.base != b.base) {
            error_message << "Pointers have different base    ";
            error_message << "left base : " << (void*) a.base << " right base : " << (void*) b.base;
            throw RangedPtrException(error_message.str());
        }
    }

    static unsigned char* ptr_check(const Ranged_Ptr& r_ptr, const unsigned char* ptr) {
        std::ostringstream error_message;

        T_index index;

        try {
            index = ptr - r_ptr.base;
        }
        catch (RangeTypeException e) {
            error_message << "Goal pointer value out of bound" << std::endl;
            error_message << "Expressed in pointers:" << std::endl;
            error_message << "Range : [ " << (void*) r_ptr.base << ", " << (void*) (r_ptr.base + index.last()) << " ]    ";
            error_message << "Goal : " << (void*) ptr << std::endl;
            error_message << "Expressed in indices:" << std::endl;
            error_message << e.what();
            throw RangedPtrException(error_message.str());
        }

        return (unsigned char*) ptr;
    }

    static unsigned char* ptr_add(const Ranged_Ptr& r_ptr, const ptr_int val) {
        std::ostringstream error_message;

        T_index index;

        try {
            index = r_ptr.cur - r_ptr.base;
            index += val;
        }
        catch (RangeTypeException e) {
            error_message << "Pointer addition results in out of bound pointer value" << std::endl;
            error_message << "Expressed in pointers:" << std::endl;
            error_message << "Range : [ " << (void*) r_ptr.base << ", " << (void*) (r_ptr.base + index.last()) << " ]    ";
            error_message << "Goal : " << (void*) (r_ptr.cur + val) << std::endl;
            error_message << "Expressed in indices:" << std::endl;
            error_message << e.what();
            throw RangedPtrException(error_message.str());
        }

        return (unsigned char*) (r_ptr.base + index.value());
    }

    static unsigned char* ptr_sub(const Ranged_Ptr& r_ptr, const ptr_int val) {
        std::ostringstream error_message;

        T_index index;

        try {
            index = r_ptr.cur - r_ptr.base;
            index -= val;
        }
        catch (RangeTypeException e) {
            error_message << "Pointer subtraction results in out of bound pointer value" << std::endl;
            error_message << "Expressed in pointers:" << std::endl;
            error_message << "Range : [ " << (void*) r_ptr.base << ", " << (void*) (r_ptr.base + index.last()) << " ]    ";
            error_message << "Goal : " << (void*) (r_ptr.cur - val) << std::endl;
            error_message << "Expressed in indices:" << std::endl;
            error_message << e.what();
            throw RangedPtrException(error_message.str());
        }

        return (unsigned char*) (r_ptr.base + index.value());
    }
};

#endif // RANGED_PTR_H
