#ifndef LISP_NUMBERS_HPP
#define LISP_NUMBERS_HPP

#include <sstream>

#include "object.hpp"

namespace lisp {
    /**
       @brief Number interface.

       Provides elementary numeric operations like
       addition, multiplication, ...
    */
    class number : public object
    {
    public:
        virtual bool is_int() const
            {
                return false;
            }

        virtual bool is_float() const
            {
                return false;
            }

        bool is_number() const
            {
                return true;
            }
    };

    /**
       @brief The integer implementation.
       @todo Implement this using libgmp to provide arbitrary
       large integers.
    */
    class int_number : public number
    {
    public:
        typedef long value_type;

        int_number(value_type v)
            : m_value(v)
            {
            }

        int_number(const std::string& str)
            {
                std::stringstream os;

                os << str;
                os >> m_value;
            }

        std::string str() const
            {
                std::stringstream os;

                os << m_value;

                return os.str();
            }

        bool is_int() const;
        value_type value() const;
        bool operator==(const object& other);

    private:
        long m_value;
    };

    class float_number : public number
    {
    public:
        typedef float value_type;

        float_number(value_type v)
            : m_value(v)
            {
            }

        float_number(const std::string& str)
            {
                std::stringstream os;

                os << str;
                os >> m_value;
            }

        std::string str() const
            {
                std::stringstream os;

                os << m_value;

                return os.str();
            }

        bool is_float() const;
        value_type value() const;
        bool operator==(const object& other);

    private:
        value_type m_value;
    };
}

#endif  // LISP_NUMBERS_HPP
