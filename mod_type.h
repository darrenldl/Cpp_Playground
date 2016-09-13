#ifndef MOD_TYPE_H_INCLUDED
#define MOD_TYPE_H_INCLUDED

template <typename T, T UB>
class Mod_Type {
public:
    Mod_Type() : val {0} {}

    Mod_Type(T a) : val {mod_val(a)} {}

    friend std::ostream& operator<< (std::ostream& out, const Mod_Type& a) {
        out << a.val;
        return out;
    }

    Mod_Type operator+ () {
        return *this;
    }

    Mod_Type operator- () {
        return mod_val(-val);
    }

    friend Mod_Type operator+ (Mod_Type a, T b) {
        return a.val + mod_val(b);
    }

    friend Mod_Type operator+ (T b, Mod_Type a) {
        return a + b;
    }

    friend Mod_Type operator- (Mod_Type a, T b) {
        return a + (-b);
    }

    friend Mod_Type operator- (T b, Mod_Type a) {
        return b + (-a);
    }

    friend Mod_Type operator* (Mod_Type a, T b) {
        return a.val * mod_val(b);
    }

    friend Mod_Type operator* (T b, Mod_Type a) {
        return a * b;
    }

    Mod_Type operator++ () {
        return *this + 1;
    }

    Mod_Type operator-- () {
        return *this - 1;
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
