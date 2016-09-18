# Cpp_Playground

Random code for fun and possible usefulness

Assumes C++11(and probably C++14) for the most part

mod_type.h and range_type.h assumes your system is one's complement or two's complement at places, specifically the number of negative values and number of non-negative values are roughly equal(off by 1 at most).

## Content

### mod_type.h
Template for modulo type, which behaves similarly to modulo type in Ada

No requirement of other libraries/headers

Usage:

    # General format
        Mod_Type<integral_type, upper_bound> variable_name;
    # Example
        Mod_Type<unsigned int, 5> i;  // by default initialised to 0
        All operations on i will be reduced modulo 5, that is, i is always in {0, 1, 2, 3, 4}
    
    # Operations supported
    Arithemetic         : +, -, *
        i + 6;  // gives 1
        i - 7;  // gives 3
    Increment/decrement : +=, -=, ++(both prefix and postfix), --(both prefix and postfix)
        i += 500;   // gives 0 still, since 500 is multiple of 5
    Comparison          : ==(congruent), !=(not congruent)
        i = 1;
        i == 111;   // returns true, since 111 is congruent to 1 modulo 5
    
    # Static asserts
        Type is asserted to be of integral type
        Upper bound is asserted to be less than or equal to maximum possible value of the given type
        Upper bound is asserted to be positive(and non-zero)
        
    # Overflow/underflow handling
        Mod_Type itself will never wrap around accidentally due to integer arithmetic
        (it will always wrap around the upper bound)
        
        Given the value provided does not overflow in the operation call,
        and causing wrap around to occur before arriving in the function,
        then all operations will not cause overflow/underflow
        
        Example:
            Mod_Type<int, 2147483647> k = 2147483646;   // assume upper limit of int is 2147483647
            k + 2147483646; // gives 2147483645, which is correct
                            // despite adding both number directly is beyond the type upper limit

### range_type.h
Template for range type, which behaves similarly to range type in Ada

Requires Mod_Type from mod_type.h

Usage:

    # General format
        Range_Type<integral_type, first_value, last_value> variable_name;
    # Example
        Range_Type<int, -5, 4> i;  // by default initialised to first_val, in this case, -5
        All operations on i will be checked for range, i must be in [-5, 4]
    
    # Operations supported
    Arithemetic         : +, -, *
        i = 10;             // not okay as it causes overflow
        i = -5; i + 6;      // gives 1
        i = 1;  i - 7;      // not okay as it causes underflow
        i = -1; i * -5;     // not okay as it causes overflow
        
    Increment/decrement : +=, -=, ++(both prefix and postfix), --(both prefix and postfix)
        i = 0; i += 2; // gives 2
        
    For-each loop
        for (auto j : i) {
            std::cout << j << endl; // prints out -5 to 3 (Note that 4 is not printed)
        }
        
        for (auto j : Range_Type<int, 0, 5>()) {
            std::cout << j << endl; // prints out 0 to 4 (Note that 5 is not printed)
        }
    
    # Static asserts
        Type is asserted to be of integral type
        First possible value is asserted to be greater than or equal to minimum possible value of the given type
        Last possible value is asserted to be less than or equal to maximum possible value of the given type
        First possible value is asserted to be less than or equal to last possible value
        
    # Overflow/underflow handling
        All overflowing/underflowing operations will throw exception RangeTypeException
        RangeTypeException.what() gives complete error message
        
        All failing operations will not result change in value of the Range_Type variable
        
        Example:
            Range_Type<int, -10, 10> k = 0;
            try {
                std::cout << k + 11 << std::endl;
            }
            catch (RangeTypeException e) {
                std::cout << e.what() << std::endl;
            }
            
            Output:
            Range : [ -10, 10 ]    Operation : 0 + 11
            Addition causes overflow
            
        Slightly more extreme case:
            Range_Type<int, -2147483648, 2147483647> k;     // assume lower limit of int is -2147483648
                                                            // and upper limit is 2147483647
            try {
                k = 2147483647;     // works, obviously
                std::cout << "k : " << k << std::endl;
                k += -2147483648;   // still works, and gives -1
                std::cout << "k : " << k << std::endl;
                k += -2147483648;   // underflows, as shown by exception
            }
            catch (RangeTypeException e) {
                std::cout << e.what() << std::endl;
            }

            Output:
            k : 2147483647
            k : -1
            Range : [ -2147483648, 2147483647 ]    Operation : -1 + (-2147483648)
            Addition causes underflow
