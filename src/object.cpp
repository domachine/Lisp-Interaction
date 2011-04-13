
#include "object.hpp"

#include <sstream>


namespace lisp {
    object_ptr_t object::operator()(environment* env, const cons_cell_ptr_t args)
    {
        return object_ptr_t();
    }

    std::string object::str() const
    {
        std::stringstream ss;

        ss << "#<object at " << this << ">";

        return ss.str();
    }

    object::operator bool() const
    {
        return true;
    }

    object_ptr_t object::eval(environment*)
    {
        return object_ptr_t();
    }
}
