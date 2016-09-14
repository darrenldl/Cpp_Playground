/* Range type template which functions similarly to range type in Ada
 * Exception is raised when the
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
#include <string>
#include <sstream>
#include <stdexcept>

#ifndef RANGE_TYPE_H_INCLUDED
#define RANGE_TYPE_H_INCLUDED

class RangeTypeException : public std::runtime_error {
public:
    RangeTypeException (std::string errMsg) : runtime_error(errMsg) {}
private:
};

template <typename T, long long int F, long long int L>
class Range_Type {

    static_assert(std::is_integral<T>::value,
                  "Type must be integral");

    static_assert(F >= std::numeric_limits<T>::min(),
                  "First is smaller than type minimum possible value");

    static_assert(L <= std::numeric_limits<T>::max(),
                  "Last exceeds type maximum possible value");

    static_assert(F <= L,
                  "First is larger than last");

public:
    Range_Type () : val {F} {}

    Range_Type (T a) : val {val_check(a)} {}

    Range_Type (const Range_Type& a) : val {val_check(a.val)} {};

    operator T () const {
        return this->val;
    }

    T value () const {
        return this->val;
    }

    T first () const {
        return this->lower_limit;
    }

    T last () const {
        return this->upper_limit;
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
        return a.val_add(a.val, b);     // b is not checked for range by putting on right
    }

    friend Range_Type operator- (const Range_Type& a, const Range_Type& b) {
        return a.val_sub(a.val, b.val);
    }

    friend Range_Type operator- (const Range_Type& a, const T& b) {
        return a.val_sub(a.val, b);
    }

    friend Range_Type operator- (const T& b, const Range_Type& a) {
        return a.val_add(a.val, -b);    // b is not checked for range by putting on right
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
        (*this) -= 1;
        return ret;
    }

    Range_Type operator-- () {
        return (*this) -= 1;
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

private:
    static const T lower_limit = F;
    static const T upper_limit  = L;
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
            error_message << "value is greater than largest possible value";
            throw RangeTypeException(error_message.str());
        }

        return val;
    }

    // all arithmetic functions do not check b for range

    T val_add (T a, T b) const {
        std::ostringstream error_message;

        val_check(a);

        const T up_space_left_a  = upper_limit - a;
        const T low_space_left_a = a           - lower_limit;

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
            b = -b;
            if (low_space_left_a < b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " + (-" << +b << ")" << std::endl;
                error_message << "Addition causes underflow";
                throw RangeTypeException(error_message.str());
            }

            a -= b;
        }

        return a;
    }

    T val_sub (T a, T b) const {
        std::ostringstream error_message;

        val_check(a);

        const T up_space_left_a  = upper_limit - a;
        const T low_space_left_a = a           - lower_limit;

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
            max_multiplier_a_pos = upper_limit / a;
            max_multiplier_a_neg = lower_limit / a;
        }
        else {
            max_multiplier_a_pos = lower_limit / a;
            max_multiplier_a_neg = upper_limit / a;
        }

        if (b >= 0) {
            if (max_multiplier_a_pos < b) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " * " << +b << std::endl;
                error_message << "Multiplication causes overflow";
                throw RangeTypeException(error_message.str());
            }

            a *= b;
        }
        else {
            if (max_multiplier_a_neg < (-b)) {
                error_message << "Range : [ " << +low_limit() << ", " << +up_limit() << " ]    ";
                error_message << "Operation : " << +a << " * " << +b << std::endl;
                error_message << "Multiplication causes underflow";
                throw RangeTypeException(error_message.str());
            }

            a *= b;
        }

        return a;
    }
};

#endif // RANGE_TYPE_H_INCLUDED
