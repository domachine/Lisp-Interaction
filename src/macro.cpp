
#include "macro.hpp"

#include <boost/foreach.hpp>

#include "utils.hpp"


namespace lisp {
    macro::macro(const arg_sym_list_t& arg_symbols,
                       cons_cell_ptr_t body)
        : object(),
          m_arg_symbols(arg_symbols),
          m_body(body)
    {
    }

    object_ptr_t macro::operator()(environment* env, const cons_cell_ptr_t args)
    {
        // Create isolated environment.
        environment func_env(env);

        cons_cell_ptr_t _args = list_next(args, args->car()->str() + ": listp");

        // Hold all given arguments to save them from garbage collection.
        std::list<symbol_ptr_t> arg_symbols;

        // Assign all given args to corresponding symbols
        // in the macro environment.
        BOOST_FOREACH(const std::string& current, m_arg_symbols) {
            if(!_args)
                signal(env->get_symbol("wrong-number-of-arguments"),
                       args->car()->str());

            // Create new symbol in macro environment.
            symbol_ptr_t new_sym = func_env.create_symbol(current);
            new_sym->set_value(_args->car());

            // Push to argument list to save from garbage collection.
            arg_symbols.push_back(new_sym);

            _args = list_next(_args, args->car()->str() + ": listp");
        }

        object_ptr_t last_result = nil();
        cons_cell_ptr_t _body = m_body;

        while(_body) {
            last_result = func_env.eval(_body->car());
            _body = list_next(_body, args->car()->str() + ": listp");
        }

        return func_env.eval(last_result);
    }

    std::string macro::str() const
    {
        std::stringstream os;

        os << "#<macro at " << this << ">";

        return os.str();
    }
}
