/* Modulo type template which functions similarly to modulo type in Ada
 *
 * Author : Darrenldl <dldldev@yahoo.com>
 *
 * License:
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

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
