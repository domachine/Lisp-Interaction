#ifndef LISP_TYPES_HPP
#define LISP_TYPES_HPP

#include "object.hpp"
#include "numbers.hpp"

namespace lisp {
    class string : public object
    {
    public:
        string(const std::string& std_str)
            : m_str(std_str)
            {
            }

        operator std::string() const
            {
                return m_str;
            }

        operator const char*() const
            {
                return m_str.c_str();
            }

        std::string str() const
            {
                return "\"" + m_str + "\"";
            }

    private:
        std::string m_str;
    };
}

#endif  // LISP_TYPES_HPP
