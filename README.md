# Cpp_Playground

Random code for fun and possible usefulness

Assumes C++11(and probably C++14) for the most part

## Content

### mod_type.h
Template for modulo type, which behaves similarly to modulo type in Ada

Usage:

    General format
        Mod_Type<integral_type, upper_bound> variable_name;
    Example
        Mod_Type<unsigned int, 5> i;  // by default initialised to 0
        All operations on i will be reduced modulo 100, i is always in {0, 1, ..., 99}
    
    Operations supported
    Arithemetic         : +, -, *
        i + 6;  // gives 1
        i - 7;  // gives 3
    Increment/decrement : +=, -=, ++(both prefix and postfix), --(both prefix and postfix)
        i += 500; // gives 0 still, since 500 is multiple of 5
    
    Static asserts
        Type is asserted to be of integral type
        Upper bound is asserted to be less than or equal to maximum possible value of the given type
        Upper bound is asserted to be positive(and non-zero)
    Overflow/underflow handling
        Mod_Type itself will never wrap around accidentally due to integer arithmetic
        (it will always wrap around the upper bound)
        
        Given the value provided does not overflow in the operation call,
        then all operations will not cause overflow/underflow
        
        Example:
            Mod_Type<int, 2147483647> k = 2147483646;   // assume upper limit of int is 65535
            k + 2147483646;  // gives 2147483645, which is correct, despite adding both number directly is beyond the type upper limit
