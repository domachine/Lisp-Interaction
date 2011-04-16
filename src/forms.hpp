#ifndef LISP_FORMS_HPP
#define LISP_FORMS_HPP

#include "utils.hpp"

namespace lisp {
    class if_form : public object
    {
    protected:

        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t _args = list_next(args, "if: listp");

                if(_args && env->eval(_args->car()) != nil()) {
                    _args = list_next(_args, "if: listp");

                    if(_args)
                        return env->eval(_args->car());
                    else
                        return nil();
                }
                else {
                    object_ptr_t result = nil();
                    _args = list_next(_args, "if: listp");

                    while(_args) {
                        _args = list_next(_args, "if: listp");

                        if(_args)
                            result = env->eval(_args->car());
                    }

                    return result;
                }
            }
    };

    class or_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t _args = list_next(args, "or: listp");

                while(_args) {
                    object_ptr_t evaled = env->eval(_args->car());

                    if(evaled != nil())
                        return evaled;

                    _args = list_next(_args, "or: listp");
                }

                return nil();
            }
    };

    class and_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                object_ptr_t last = nil();
                cons_cell_ptr_t _args = list_next(args, "and: listp");

                while(_args) {
                    last = env->eval(_args->car());

                    if(last == nil())
                        return last;

                    _args = list_next(_args, "and: listp");
                }

                return last;
            }
    };

    class print_function : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t _args = list_next(args, "print: listp");

                while(_args) {
                    std::cerr << env->eval(_args->car())->str() << std::endl;

                    _args = list_next(_args, "print: listp");
                }

                return nil();
            }
    };

    class fset_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t _args = list_next(args, "fset: listp");

                object_ptr_t sym_ref = env->eval(_args->car());

                if(!sym_ref->is_symbol_ref())
                    signal(env->get_symbol("wrong-type-argument"),
                           "fset: symbolp");

                symbol_ref_ptr_t r_sym_ref = boost::dynamic_pointer_cast<symbol_ref>(sym_ref);

                symbol_ptr_t sym = env->get_symbol(r_sym_ref->name());

                sym->set_function(env->eval(list_next(_args, "fset: listp")->car()));

                return sym;
            }
    };
}

#endif  // LISP_FORMS_HPP
