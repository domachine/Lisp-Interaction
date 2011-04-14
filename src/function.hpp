#ifndef LISP_FUNCTION_HPP
#define LISP_FUNCTION_HPP

#include <list>

#include "lisp.hpp"

namespace lisp {
    /**
       @brief Holds a lisp function created through a defun-form.

       e.g. (defun hello-world () ...)
    */
    class function : public object
    {
    public:
        // Type to hold the parameter-names.
        typedef std::list<std::string> arg_sym_list_t;

        /**
           @brief Instantiates a new function.

           @param arg_symbols A list of the parameter-names that can
           be passed to the function.

           @param body The function body as compiled list.
        */
        function(const arg_sym_list_t& arg_symbols, cons_cell_ptr_t body);

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

    /**
       @brief The function object which is hold in the function-cell
       of the symbol `defun' in the global environment.
     */
    class defun_form : public object
    {
    public:
        object_ptr_t operator()(environment* env, const cons_cell_ptr_t args);
    };
}

#endif  // LISP_FUNCTION_HPP
