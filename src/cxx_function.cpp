
#include "cxx_function.hpp"

#include "lisp.hpp"
#include "utils.hpp"

namespace lisp {
    object_ptr_t cxx_function::operator()(environment* env,
                                          const cons_cell_ptr_t args)
    {
        assert(args);
        object_ptr_t func = args->car();
        std::string errmsg = func->str() + ": listp";

        //in cxx_function.hpp:
        //typedef std::vector<object_ptr_t> argv_t;
        argv_t vargs;

        cons_cell_ptr_t _args = list_next(args, errmsg);

        while(_args) {
            vargs.push_back(env->eval(_args->car()));

            _args = list_next(_args, errmsg);
        }

        return (*this)(env, vargs);
    }
}
