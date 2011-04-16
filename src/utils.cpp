
#include "utils.hpp"


namespace lisp {
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

    cons_cell_ptr_t list_next(cons_cell_ptr_t list, const std::string& msg)
    {
        object_ptr_t cdr = list->cdr();

        if(cdr->is_cons_cell())
        {
            cons_cell_ptr_t cell = boost::dynamic_pointer_cast<cons_cell>(cdr);
            return cell;
        }
        else if(cdr == nil())
            return cons_cell_ptr_t();
        else
            signal(global_env()->get_symbol("invalid-type-argument"), msg);

        assert(false);
    }
}
