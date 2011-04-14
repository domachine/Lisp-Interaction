#ifndef LISP_UTILS_HPP
#define LISP_UTILS_HPP

#include <boost/function.hpp>
namespace lisp {
    typedef boost::function<void (object_ptr_t, int)> callback_t;

    void dolist(const cons_cell_ptr_t list, callback_t cb)
    {
        // const correctness
        cons_cell_ptr_t _list = list;

        int i;
        for(i = 0; _list && *_list; ++i) {
            object_ptr_t current = _list->car();

            cb(current, i);

            object_ptr_t next = _list->cdr();
            if(!next->is_cons_cell())
                // If next is not a list throw its index.
                throw i;

            _list = boost::dynamic_pointer_cast<cons_cell>(next);
        }
    }
}

#endif  // LISP_UTILS_HPP
