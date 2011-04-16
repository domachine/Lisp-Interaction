#ifndef LISP_UTILS_HPP
#define LISP_UTILS_HPP

#include <boost/function.hpp>

#include "lisp.hpp"


namespace lisp {
    typedef boost::function<void (object_ptr_t, int)> callback_t;

    void dolist(const cons_cell_ptr_t list, callback_t cb);

    cons_cell_ptr_t list_next(cons_cell_ptr_t list,
                              const std::string& msg = std::string());
}

#endif  // LISP_UTILS_HPP
