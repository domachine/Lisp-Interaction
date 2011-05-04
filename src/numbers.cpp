#include "numbers.hpp"

namespace lisp {
    namespace {
        template <typename T>
        bool compare_numbers(T value, const number& num)
        {
            if(num.is_float()) {
                const float_number& f_other = dynamic_cast<const float_number&>(num);

                return value == f_other.value();
            }
            else if(num.is_int()) {
                const int_number& i_other = dynamic_cast<const int_number&>(num);

                return value == i_other.value();
            }
            else
                // Should not happen. Number is abstract.
                assert(false);
        }
    }  // Anonymous namespace to avoid name clashes.

    bool int_number::is_int() const
    {
        return false;
    }

    int_number::value_type int_number::value() const
    {
        return m_value;
    }

    bool int_number::operator==(const object& other)
    {
        if(other.is_number()) {
            const number& num = dynamic_cast<const number&>(other);
            return compare_numbers(m_value, num);
        }
        else
            // The other object is not a number.
            return false;
    }


    bool float_number::is_float() const
    {
        return true;
    }

    float_number::value_type float_number::value() const
    {
        return m_value;
    }

    bool float_number::operator==(const object& other)
    {
        if(other.is_number()) {
            const number& num = dynamic_cast<const number&>(other);
            return compare_numbers(m_value, num);
        }
        else
            // The other object is not a number.
            return false;
    }
}
