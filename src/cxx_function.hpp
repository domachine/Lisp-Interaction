#ifndef LISP_CXX_FUNCTION_HPP
#define LISP_CXX_FUNCTION_HPP

#include "object.hpp"

namespace lisp {
    class cxx_function : public object
    {
    protected:
        typedef std::vector<object_ptr_t> argv_t;
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t());

        virtual object_ptr_t operator()(environment* env,
                                        const argv_t& args) = 0;
    };
}

#endif  // LISP_CXX_FUNCTION_HPP
