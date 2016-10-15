/* Range type template which functions similarly to range type in Ada
 * Exception RangeTypeException is thrown when overflow/underflow occurs
 * RangeTypeException.what() contains the full error message
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

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "mod_type.h"

#ifndef RANGE_TYPE_H_INCLUDED
#define RANGE_TYPE_H_INCLUDED

class RangeTypeException : public std::runtime_error {
public:
    RangeTypeException (std::string errMsg) : runtime_error(errMsg) {}
private:
};

template <typename T, long long int F, long long int L>
class Range_Type : public std::iterator<std::random_access_iterator_tag,
                                        T,
                                        T,
                                        const T*,
                                        T
                                       >
{

    static_assert(std::is_integral<T>::value,
                  "Type must be integral");

    static_assert(F >= std::numeric_limits<T>::min(),
                  "First is smaller than type minimum possible value");

    static_assert(L <= std::numeric_limits<T>::max(),
                  "Last exceeds type maximum possible value");

    static_assert(F <= L,
                  "First is larger than last");

private:
    using rand_iterator = std::iterator<std::random_access_iterator_tag, T, T, const T*, T>;

public:
    // iterator functions
    static Range_Type begin () {
        return Range_Type(F);
    }

    static Range_Type end () {
        return Range_Type(L);
    }

    typename rand_iterator::reference operator* () const {
        return val;
    }

    // class constructors and other functions
    Range_Type () : val {F} {}

    Range_Type (T a) : val {val_check(a)} {}

    Range_Type (const Range_Type& a) : val {val_check(a.val)} {};

    Range_Type operator= (const Range_Type& a) {
        this->val = val_check(a.val);
        return *this;
    }

    Range_Type operator= (const T& a) {
        this->val = val_check(a);
        return *this;
    }

    template<typename ANY_T>
    operator ANY_T () const = delete;

    T value () const {
        return this->val;
    }

    T first () const {
        return this->lower_limit;
    }

    T last () const {
        return this->upper_limit;
    }

    T size () const {
        return this->upper_limit - this->lower_limit + 1;
    }

    friend std::ostream& operator<< (std::ostream& out, const Range_Type& a) {
        out << +a.val;
        return out;
    }

    Range_Type operator+ () const {
        return *this;
    }

    Range_Type operator- () const {
        return val_check(-val);
    }

    friend Range_Type operator+ (const Range_Type& a, const Range_Type& b) {
        return a.val_add(a.val, b.val);
    }

    friend Range_Type operator+ (const Range_Type& a, const T& b) {
        return a.val_add(a.val, b);
    }

    friend Range_Type operator+ (const T& b, const Range_Type& a) {
        return a.val_add(a.val, b);
    }

    friend Range_Type operator- (const Range_Type& a, const Range_Type& b) {
        return a.val_sub(a.val, b.val);
    }

    friend Range_Type operator- (const Range_Type& a, const T& b) {
        return a.val_sub(a.val, b);
    }

    friend Range_Type operator- (const T& b, const Range_Type& a) {
        return a.val_sub(a.val, b);
    }

    friend Range_Type operator* (const Range_Type& a, const Range_Type& b) {
        return a.val_mul(a.val, b.val);
    }

    friend Range_Type operator* (const Range_Type& a, const T& b) {
        return a.val_mul(a.val, b);
    }

    friend Range_Type operator* (const T& b, const Range_Type& a) {
        return a.val_mul(a.val, b);     // b is not checked for range by putting on right
    }

    Range_Type operator++ () {
        return (*this) += 1;
    }

    Range_Type operator++ (int) {
        T ret = val;
        (*this) += 1;
        return ret;
    }

    Range_Type operator-- () {
        return (*this) -= 1;
    }

    Range_Type operator-- (int) {
        T ret = val;
        (*this) -= 1;
        return ret;
    }

    Range_Type operator+= (const Range_Type& a) {
        this->val = val_add(this->val, a.val);
        return *this;
    }

    Range_Type operator+= (const T& a) {
        this->val = val_add(val, a);
        return *this;
    }

    Range_Type operator-= (const Range_Type& a) {
        this->val = val_sub(this->val, a.val);
        return *this;
    }

    Range_Type operator-= (const T& a) {
        this->val = val_sub(val, a);
        return *this;
    }

    Range_Type operator*= (const Range_Type& a) {
        this->val = val_mul(this->val, a.val);
        return *this;
    }

    Range_Type operator*= (const T& a) {
        this->val = val_mul(val, a);
        return *this;
    }

    friend bool operator== (const Range_Type& a, const Range_Type& b) {
        return a.val == b.val;
    }

    friend bool operator!= (const Range_Type& a, const Range_Type& b) {
        return a.val != b.val;
    }

private:
    // Spill_Proof is for internal use only
    // it still overflows/underflows but only when the value is outside the range [-T_max^T_max, T_max^T_max]
    // T_max is maximum possible value of type, ^ used above is power notation rather than bitwise XOR operation
    class Spill_Proof {
    private:
        static const T T_max = std::numeric_limits<T>::max();

    public:
        Spill_Proof ()      : multiplier {0}, remainder {0} {}

        Spill_Proof (T val) : multiplier {0} {
            multiplier = val / T_max;
            remainder = val;
        }

        Spill_Proof (const Spill_Proof& val) : multiplier {val.multiplier},
                                               remainder {val.remainder} {}

        Spill_Proof operator= (const Spill_Proof& a) {
            this->multiplier = a.multiplier;
            this->remainder  = a.remainder;

            return *this;
        }

        Spill_Proof operator= (const T a) {
            *this = Spill_Proof(a);

            return *this;
        }

        friend std::ostream& operator<< (std::ostream& out, const Spill_Proof& a) {
            out << +T_max << " * " << +a.multiplier << " + " << +a.remainder;
            return out;
        }

        friend Spill_Proof operator+ (const Spill_Proof& a, const Spill_Proof& b) {
            Spill_Proof result;

            result.multiplier = a.multiplier + b.multiplier;
            result.remainder  = a.remainder;
            if (T_max - result.remainder.value() < b.remainder.value()) {
                result.multiplier++;
            }
            result.remainder += b.remainder;

            return result;
        }

        friend Spill_Proof operator+ (const Spill_Proof& a, const T b) {
            return a + Spill_Proof(b);
        }

        friend Spill_Proof operator+ (const T b, const Spill_Proof& a) {
            return a + b;
        }

        friend Spill_Proof operator- (const Spill_Proof& a, const Spill_Proof& b) {
            Spill_Proof result;

            result.multiplier = a.multiplier - b.multiplier;
            result.remainder  = a.remainder;
            if (result.remainder.value() < b.remainder.value()) {
                result.multiplier--;
            }
            result.remainder -= b.remainder;

            return result;
        }

        friend Spill_Proof operator- (const Spill_Proof& a, const T b) {
            return a - Spill_Proof(b);
        }

        friend Spill_Proof operator- (const T b, const Spill_Proof& a) {
            return a - b;
        }

        Spill_Proof operator+= (const Spill_Proof& a) {
            *this = *this + a;
            return *this;
        }

        Spill_Proof operator+= (const T a) {
            *this = *this + a;
            return *this;
        }

        Spill_Proof operator-= (const Spill_Proof& a) {
            *this = *this - a;
            return *this;
        }

        Spill_Proof operator-= (const T a) {
            *this = *this - a;
            return *this;
        }

        friend bool operator< (const Spill_Proof& a, const Spill_Proof& b) {
            if (a.multiplier == b.multiplier) {
                return a.remainder.value() < b.remainder.value();
            }
            else {
                return a.multiplier < b.multiplier;
            }
        }

        friend bool operator< (const Spill_Proof& a, const T b) {
            return a < Spill_Proof(b);
        }

        friend bool operator< (const T b, const Spill_Proof& a) {
            return a < b;
        }

        friend bool operator>= (const Spill_Proof& a, const Spill_Proof& b) {
            return !(a < b);
        }

        friend bool operator>= (const Spill_Proof& a, const T b) {
            return !(a < b);
        }

        friend bool operator>= (const T b, const Spill_Proof& a) {
            return !(a < b);
        }

        friend bool operator> (const Spill_Proof& a, const Spill_Proof& b) {
            if (a.multiplier == b.multiplier) {
                return a.remainder.value() > b.remainder.value();
            }
            else {
                return a.multiplier > b.multiplier;
            }
        }

        friend bool operator<= (const Spill_Proof& a, const Spill_Proof& b) {
            return !(a > b);
        }

        friend bool operator== (const Spill_Proof& a, const Spill_Proof& b) {
            if (a.multiplier == b.multiplier) {
                return a.remainder.value() == b.remainder.value();
            }
            else {
                return false;
            }
        }

    private:
        T multiplier;
        Mod_Type<T, T_max> remainder;
    };

    static const T lower_limit = F;
    static const T upper_limit = L;
    T val;

    static T low_limit () {
        return F;
    }

    static T up_limit () {
        return L;
    }

    static T val_check (const T val) {
        std::ostringstream error_message;

        if (val < F) {
            error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
            error_message << "Goal : "    << +val           << std::endl;
            error_message << "Value is lower than smallest possible value";
            throw RangeTypeException(error_message.str());
        }

        if (val > L) {
            error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
            error_message << "Goal : "    << +val           << std::endl;
            error_message << "Value is greater than largest possible value";
            throw RangeTypeException(error_message.str());
        }

        return val;
    }

    // all arithmetic functions do not check b for range

    T val_add (T a, T b) const {
        std::ostringstream error_message;

        val_check(a);

        Spill_Proof up_space_left_a  = Spill_Proof(upper_limit) - Spill_Proof(a);
        Spill_Proof low_space_left_a = Spill_Proof(a)           - Spill_Proof(lower_limit);

        if (b >= 0) {   // normal addition
            if (up_space_left_a < b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " + " << +b << std::endl;
                error_message << "Addition causes overflow";
                throw RangeTypeException(error_message.str());
            }

            a += b;
        }
        else {  // subtraction
            T t_val = 0;

            if (b == std::numeric_limits<T>::min()) {
                // prevent negating from wrapping back to same value
                // this is to deal with case where number of negative values is one greater than number of non-negative values
                // aka two's complement
                t_val = 1;      // put 1 aside to avoid overflow in negation
                b += t_val;

                b = -b;
                if (low_space_left_a == 0) {
                    error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                    error_message << "Operation : " << +a << " + (" << +std::numeric_limits<T>::min() << ")" << std::endl;
                    error_message << "Addition causes underflow";
                    throw RangeTypeException(error_message.str());
                }
                else if (low_space_left_a > 0) {
                    if (low_space_left_a - t_val < b) {
                        error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                        error_message << "Operation : " << +a << " + (" << +std::numeric_limits<T>::min() << ")" << std::endl;
                        error_message << "Addition causes underflow";
                        throw RangeTypeException(error_message.str());
                    }
                }
                /* low_space_left_a < 0 will not hold due to val_check at front */
            }
            else {
                b = -b;
                if (low_space_left_a < b) {
                    error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                    error_message << "Operation : " << +a << " + (-" << +b << ")" << std::endl;
                    error_message << "Addition causes underflow";
                    throw RangeTypeException(error_message.str());
                }
            }

            a -= b;
            a -= t_val;
        }

        return a;
    }

    T val_sub (T a, T b) const {
        std::ostringstream error_message;

        val_check(a);

        Spill_Proof up_space_left_a  =  Spill_Proof (upper_limit) - Spill_Proof(a);
        Spill_Proof low_space_left_a =  Spill_Proof(a)            - Spill_Proof(lower_limit);

        if (b >= 0) {   // normal subtraction
            if (low_space_left_a < b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " - " << +b << std::endl;
                error_message << "Subtraction causes underflow";
                throw RangeTypeException(error_message.str());
            }

            a -= b;
        }
        else {  // addition
            b = -b;
            if (up_space_left_a < b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " - (-" << +b << ")" << std::endl;
                error_message << "Subtraction causes overflow";
                throw RangeTypeException(error_message.str());
            }

            a += b;
        }

        return a;
    }

    T val_mul (T a, T b) const {
        std::ostringstream error_message;

        val_check(a);

        T max_multiplier_a_pos;
        T max_multiplier_a_neg;

        if (a >= 0) {
            if (upper_limit >= 0) {
                max_multiplier_a_pos = upper_limit / a;
            }
            else {
                max_multiplier_a_pos = 0;
            }

            if (lower_limit >= 0) {
                max_multiplier_a_neg = 0;
            }
            else {
                max_multiplier_a_neg = lower_limit / a;
            }
        }
        else {
            if (upper_limit >= 0) {
                max_multiplier_a_neg = upper_limit / a;
            }
            else {
                max_multiplier_a_neg = 0;
            }

            if (lower_limit >= 0) {
                max_multiplier_a_pos = 0;
            }
            else {
                max_multiplier_a_pos = lower_limit / a;
            }
        }

        if (b >= 0) {
            if (max_multiplier_a_pos < b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " * " << +b << std::endl;
                if (a >= 0) {
                    error_message << "Multiplication causes overflow";
                }
                else {
                    error_message << "Multiplication causes underflow";
                }
                throw RangeTypeException(error_message.str());
            }

            a *= b;
        }
        else {
            if (max_multiplier_a_neg > b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " * " << +b << std::endl;
                if (a >= 0) {
                    error_message << "Multiplication causes underflow";
                }
                else {
                    error_message << "Multiplication causes overflow";
                }
                throw RangeTypeException(error_message.str());
            }

            a *= b;
        }

        return a;
    }
};

#endif // RANGE_TYPE_H_INCLUDED
