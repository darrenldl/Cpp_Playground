#include <limits>

#ifndef MOD_TYPE_H_INCLUDED
#define MOD_TYPE_H_INCLUDED

template <typename T, long int UB>
class Mod_Type {

    static_assert(std::is_integral<T>::value,
                  "Type must be integral");

    static_assert(UB <= std::numeric_limits<T>::max(),
                  "Upper bound exceeds type maximum possible value");

    static_assert(UB > 0,
                  "Upper bound is not positive(and non-zero)");

public:
    Mod_Type() : val {0} {}

    Mod_Type(T a) : val {mod_val(a)} {}

    T value () const {
        return this->val;
    }

    friend std::ostream& operator<< (std::ostream& out, const Mod_Type& a) {
        out << a.val;
        return out;
    }

    Mod_Type operator+ () const {
        return *this;
    }

    Mod_Type operator- () const {
        return mod_val(-val);
    }

    friend Mod_Type operator+ (const Mod_Type& a, const Mod_Type& b) {
        return a.val + b.val;
    }

    friend Mod_Type operator+ (const Mod_Type& a, const T& b) {
        return a.val + mod_val(b);
    }

    friend Mod_Type operator+ (const T& b, const Mod_Type& a) {
        return a + b;
    }

    friend Mod_Type operator- (const Mod_Type& a, const T& b) {
        return a + (-b);
    }

    friend Mod_Type operator- (const T& b, const Mod_Type& a) {
        return b + (-a);
    }

    friend Mod_Type operator* (const Mod_Type& a, const T& b) {
        return a.val * mod_val(b);
    }

    friend Mod_Type operator* (const T& b, const Mod_Type& a) {
        return a * b;
    }

    Mod_Type operator++ () {
        return (*this) += 1;
    }

    Mod_Type operator++ (int) {
        T ret = val;
        (*this) += 1;
        return ret;
    }

    Mod_Type operator-- () {
        return (*this) -= 1;
    }

    Mod_Type operator-- (int) {
        T ret = val;
        (*this) -= 1;
        return ret;
    }

    Mod_Type operator+= (const Mod_Type& a) {
        this->val = mod_val(this->val + a.val);
        return *this;
    }

    Mod_Type operator+= (const T& a) {
        val = mod_val(val + mod_val(a));
        return *this;
    }

    Mod_Type operator-= (const Mod_Type& a) {
        return (*this) += -a;
    }

    Mod_Type operator-= (const T& a) {
        return (*this) += -a;
    }

private:
    static const T upper_bound = UB;
    T val;

    static T mod_val (T a) {
        if (a < 0) {
            a = -a;
            a = a % upper_bound;
            a = upper_bound - a;
        }
        else {
            a = a % upper_bound;
        }

        return a;
    }
};

#endif // MOD_TYPE_H_INCLUDED
