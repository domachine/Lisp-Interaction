#ifndef LISP_FORMS_HPP
#define LISP_FORMS_HPP

#include "utils.hpp"
#include "cxx_function.hpp"

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

                if(!_args)
                    signal(env->get_symbol("wrong-number-of-arguments"), "setf");

                object_ptr_t sym_ref = env->eval(_args->car());

                if(!sym_ref->is_symbol_ref())
                    signal(env->get_symbol("wrong-type-argument"),
                           "fset: symbolp");

                symbol_ref_ptr_t r_sym_ref = boost::dynamic_pointer_cast<symbol_ref>(sym_ref);

                symbol_ptr_t sym = env->get_symbol(r_sym_ref->name());

                cons_cell_ptr_t value = list_next(_args, "fset: listp");

                if(!value)
                    signal(env->get_symbol("wrong-number-of-arguments"), "fset");

                sym->set_function(env->eval(value->car()));

                return sym;
            }
    };

    class setf_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t _args = list_next(args, "setf: listp");

                if(!_args)
                    signal(env->get_symbol("wrong-number-of-arguments"), "setf");

                object_ptr_t sym_ref = env->eval(_args->car());

                if(!sym_ref->is_symbol_ref())
                    signal(env->get_symbol("wrong-type-argument"),
                           "setf: symbolp");

                symbol_ref_ptr_t r_sym_ref = boost::dynamic_pointer_cast<symbol_ref>(sym_ref);

                symbol_ptr_t sym = env->get_symbol(r_sym_ref->name());

                cons_cell_ptr_t value = list_next(_args, "setf: listp");

                if(!value)
                    signal(env->get_symbol("wrong-number-of-arguments"), "setf");

                sym->set_value(env->eval(value->car()));

                return sym;
            }
    };

    class setq_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t _args = list_next(args, "setq: listp");

                if(!_args)
                    signal(env->get_symbol("wrong-number-of-arguments"), "setq");

                object_ptr_t sym_ref = _args->car();

                if(!sym_ref->is_symbol_ref())
                    signal(env->get_symbol("wrong-type-argument"),
                           "setq: symbolp");

                symbol_ref_ptr_t r_sym_ref = boost::dynamic_pointer_cast<symbol_ref>(sym_ref);

                symbol_ptr_t sym = env->get_symbol(r_sym_ref->name());

                cons_cell_ptr_t value = list_next(_args, "setq: listp");

                if(!value)
                    signal(env->get_symbol("wrong-number-of-arguments"), "setq");

                sym->set_value(env->eval(value->car()));

                return sym;
            }
    };

    class defun_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env, const cons_cell_ptr_t args)
            {
                // Pop function object from top.
                cons_cell_ptr_t cdr = list_next(args, "defun: listp");

                if(!cdr)
                    signal(env->get_symbol("wrong-number-of-arguments"),
                           "defun");

                // Determine function symbol.
                object_ptr_t sym_raw = cdr->car();
                if(!sym_raw->is_symbol_ref())
                    signal(env->get_symbol("wrong-type-argument"),
                           "symbolp <argument-1 to defun>");

                symbol_ref_ptr_t sym_ref = boost::dynamic_pointer_cast<symbol_ref>(sym_raw);
                symbol_ptr_t sym = env->get_symbol(sym_ref->name());

                cdr = list_next(cdr, "defun: listp");

                if(!cdr)
                    signal(env->get_symbol("wrong-number-of-arguments"),
                           "defun");

                object_ptr_t arg_list = cdr->car();

                if(!arg_list->is_cons_cell() && arg_list != nil())
                    signal(env->get_symbol("wrong-type-argument"),
                           "listp <argument-2 to defun>");

                // Build functions from remaining arguments.
                function::arg_sym_list_t function_arg_list;
                cons_cell_ptr_t body = boost::dynamic_pointer_cast<cons_cell>(cdr->cdr());

                if(arg_list->is_cons_cell()) {
                    cons_cell_ptr_t arg_list_cell =
                        boost::dynamic_pointer_cast<cons_cell>(arg_list);

                    while(arg_list_cell && *arg_list_cell) {
                        if(arg_list_cell->car()->is_symbol_ref()) {
                            symbol_ref_ptr_t arg_sym =
                                boost::dynamic_pointer_cast<symbol_ref>(arg_list_cell->car());

                            function_arg_list.push_back(arg_sym->name());

                            arg_list_cell =
                                boost::dynamic_pointer_cast<cons_cell>(arg_list_cell->cdr());
                        }
                    }
                }

                // Manipulate symbol and return it.
                sym->set_function(object_ptr_t(new function(function_arg_list, body)));
                return sym;
            }
    };

    class equal_form : public object
    {
    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t())
            {
                cons_cell_ptr_t first = list_next(args, "equal: listp");

                if(!first)
                    signal(env->get_symbol("wrong-number-of-arguments"),
                           "equal");

                cons_cell_ptr_t second = list_next(first, "equal: listp");

                if(!second)
                    signal(env->get_symbol("wrong-number-of-arguments"),
                           "equal");

                if(env->eval(first->car()) == env->eval(second->car()))
                    return t();
                else
                    return nil();
            }
    };

	class plus_form : public cxx_function
	{
		object_ptr_t operator()(environment* env,
                                const argv_t& args)
			{
				float sum = 0.0;
				size_t sz = args.size();
				bool sum_is_int = true;

				for(unsigned int i = 0; i < sz; i++)
				{
					if(args[i]->is_number())
					{
						number_ptr_t num = boost::dynamic_pointer_cast<lisp::number>(args[i]);
						if(num->is_int())
						{
							sum += boost::dynamic_pointer_cast<lisp::int_number>(num)->value();
						}
						else if(num->is_float())
						{
							sum += boost::dynamic_pointer_cast<lisp::float_number>(num)->value();
							sum_is_int = false;
						}
					}

					else
					{
						//I have no idea what I should write instead of ???
						signal(env->get_symbol("wrong-type-argument"), "+: ???");
					}
				}

				if(sum_is_int)
					return int_number_ptr_t(new int_number(sum));

				return float_number_ptr_t(new float_number(sum));
			}
	};
}

#endif  // LISP_FORMS_HPP
