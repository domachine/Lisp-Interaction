#ifndef LISP_NUMBERS_HPP
#define LISP_NUMBERS_HPP

namespace lisp {
    template <typename T>
    class number : public object
    {
    public:
        number(T value)
            : m_value(value)
            {
            }

        number(const std::string& str)
            {
                std::stringstream os;

                os << str;
                os >> m_value;
            }

        bool is_number() const
            {
                return true;
            }

        operator T() const
            {
                return m_value;
            }

        std::string str() const
            {
                std::stringstream os;

                os << m_value;

                return os.str();
            }

    private:
        T m_value;
    };
}

#endif  // LISP_NUMBERS_HPP
