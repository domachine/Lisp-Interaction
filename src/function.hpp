#ifndef LISP_FUNCTION_HPP
#define LISP_FUNCTION_HPP

#include <list>

#include "lisp.hpp"

namespace lisp {
    class function : public object
    {
    public:
        typedef std::list<std::string> arg_sym_list_t;

        function(const arg_sym_list_t& arg_symbols, cons_cell_ptr_t body);

        object_ptr_t operator()(environment* env, const cons_cell_ptr_t args);

        std::string str() const;

    private:
        arg_sym_list_t m_arg_symbols;
        cons_cell_ptr_t m_body;
    };

    class defun_form : public object
    {
    public:
        object_ptr_t operator()(environment* env, const cons_cell_ptr_t args);
    };
}

#endif  // LISP_FUNCTION_HPP
