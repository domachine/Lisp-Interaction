
#ifndef LISP_ARITH_ERROR_HPP
#define LISP_ARITH_ERROR_HPP

namespace lisp {
    /**
       @brief Thrown after some mathematical error occured.
    */
    class arith_error : public std::runtime_error
    {
    public:
        arith_error(const std::string& msg)
            : std::runtime_error(msg)
            {
            }
    };
}

#endif  // LISP_ARITH_ERROR_HPP
