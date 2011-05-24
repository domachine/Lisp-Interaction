#ifndef LISP_MACRO_HPP
#define LISP_MACRO_HPP

#include <list>

#include "lisp.hpp"


namespace lisp {
    /**
       @brief Holds a lisp macro created through a defun-form.

       e.g. (defun hello-world () ...)
    */
    class macro : public object
    {
    public:
        // Type to hold the parameter-names.
        typedef std::list<std::string> arg_sym_list_t;

        /**
           @brief Instantiates a new macro.

           @param arg_symbols A list of the parameter-names that can
           be passed to the macro.

           @param body The macro body as compiled list.
        */
        macro(const arg_sym_list_t& arg_symbols, cons_cell_ptr_t body);

        /**
           @brief Sets the parameter symbols and evaluates the body.

           TODO: Implement a return form.

           @return The last result.
         */
        object_ptr_t operator()(environment* env, const cons_cell_ptr_t args);

        std::string str() const;

    private:
        arg_sym_list_t m_arg_symbols;
        cons_cell_ptr_t m_body;
    };
}

#endif  // LISP_MACRO_HPP
