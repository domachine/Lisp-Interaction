
#include "cxx_form.hpp"

#include "lisp.hpp"
#include "utils.hpp"


namespace lisp {
    cxx_function_callback::cxx_function_callback(callback_t cb)
	: m_cb(cb)
    {
    }

    object_ptr_t cxx_function_callback::operator()(environment* env,
						   const cons_cell_ptr_t args)
    {
	assert(args);
        object_ptr_t func = args->car();
        std::string errmsg = func->str() + ": listp";

        argv_t vargs;

        cons_cell_ptr_t _args = list_next(args, errmsg);

        while(_args) {
            vargs.push_back(env->eval(_args->car()));

            _args = list_next(_args, errmsg);
        }

        return (*m_cb)(env, vargs);
    }
}
