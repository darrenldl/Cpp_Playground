/* Modulo type template which functions similarly to modulo type in Ada
 * No integer overflow/underflow should occur during operations of this type
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

template <typename T, long long int UB>
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

    Mod_Type(const Mod_Type& a) : val {mod_val(a.val)} {}

    operator T() const {
        return this->val;
    }

    T value () const {
        return this->val;
    }

    T bound () const {
        return this->upper_bound;
    }

    friend std::ostream& operator<< (std::ostream& out, const Mod_Type& a) {
        out << +a.val;
        return out;
    }

    Mod_Type operator+ () const {
        return *this;
    }

    Mod_Type operator- () const {
        return mod_val(-val);
    }

    friend Mod_Type operator+ (const Mod_Type& a, const Mod_Type& b) {
        return mod_add(a.val, b.val);
    }

    friend Mod_Type operator+ (const Mod_Type& a, const T& b) {
        return mod_add(a.val, b);
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

    friend Mod_Type operator* (const Mod_Type& a, const Mod_Type& b) {
        return mod_mul(a.val, b.val);
    }

    friend Mod_Type operator* (const Mod_Type& a, const T& b) {
        return mod_mul(a.val, b);
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
        this->val = mod_add(this->val, a.val);
        return *this;
    }

    Mod_Type operator+= (const T& a) {
        this->val = mod_add(val, mod_val(a));
        return *this;
    }

    Mod_Type operator-= (const Mod_Type& a) {
        return (*this) += -a;
    }

    Mod_Type operator-= (const T& a) {
        return (*this) += -a;
    }

    Mod_Type operator *= (const Mod_Type& a) {
        this->val = mod_mul(this->val, a.val);
        return *this;
    }

    Mod_Type operator *= (const T& a) {
        this->val = mod_mul(this->val, a);
        return *this;
    }

private:
    static const T upper_bound = UB;
    T val;

    static T mod_val (T a) {
        if (a < 0) {
            if (a == std::numeric_limits<T>::min()) {
                // prevent negating from wrapping back to same value
                // this is to deal with case where number of negative values is one greater than number of non-negative values
                // aka two's complement
                a += upper_bound;   // would not change the result in modulo arithmetic
            }

            a = -a;
            a = a % upper_bound;
            if (a != 0) {
                a = upper_bound - a;
            }
        }
        else {
            a = a % upper_bound;
        }

        return a;
    }

    static T mod_add (T a, T b) {
        a = mod_val(a);
        b = mod_val(b);

        T space_left_a;

        while (true) {
            space_left_a = std::numeric_limits<T>::max() - a;

            if (space_left_a < b) { // overflow
                // b must be <= upper_bound due to mod_val above
                // b also > upper_bound - a, since b > space_left_a
                // and space_left_a >= upper_bound - a

                // add portion of b to a that results in 0
                b -= upper_bound - a;
                a = 0;
            }
            else {
                a += b;
                break;
            }
        }

        return mod_val(a);
    }

    static T mod_mul (T a, T b) {
        a = mod_val(a);
        b = mod_val(b);

        if (a == 0 || b == 0) {
            return 0;
        }

        T acc = 0;

        const T max_mutliplier_a = std::numeric_limits<T>::max() / a;   // deliberate truncation

        while (true) {
            if (max_mutliplier_a < b) { // overflow
                // multiply by mutlipliable portion first
                acc = mod_add(acc, mod_val(a * max_mutliplier_a));
                b -= max_mutliplier_a;
            }
            else {
                acc = mod_add(acc, a * b);
                break;
            }
        }

        return mod_val(acc);
    }
};

#endif // MOD_TYPE_H_INCLUDED
