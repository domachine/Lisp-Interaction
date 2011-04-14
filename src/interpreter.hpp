#ifndef LISP_INTERPRETER_HPP
#define LISP_INTERPRETER_HPP

#include "logging.hpp"

#include "lisp.hpp"
#include "tokenizer.hpp"

namespace lisp {
    namespace interpreter
    {
        template <typename T>
        object_ptr_t compile_expr(environment* env, tokenizer<T>& tok);

        template <typename T>
        object_ptr_t compile_list(environment* env, tokenizer<T>& tok)
        {
            using logging::log;
            using logging::DEBUG;

            token lisp_token = tok.next_token();

            object_ptr_t car, cdr;

            if(lisp_token == RIGHT_PARENTHESIS)
                return nil();
            else if(lisp_token == DOT) {
                // Fetch next token to skip dot.
                tok.next_token();
                return compile_expr(env, tok);
            }
            else {
                car = compile_expr(env, tok);
                //assert(*car);

                cdr = compile_list(env, tok);
            }

            cons_cell_ptr_t list(new cons_cell(car, cdr));

            return list;
        }

        template <typename T>
        object_ptr_t compile_expr(environment* env, tokenizer<T>& tok)
        {
            token lisp_token = tok.current_token();

            switch(lisp_token) {
            case LEFT_PARENTHESIS:
                return compile_list(env, tok);
            case SYMBOL:
                if(tok.value() == "nil")
                    return nil();
                else if(tok.value() == "t")
                    return t();

                return object_ptr_t(new symbol_ref(tok.value()));
            case STRING:
                return object_ptr_t(new string(tok.value()));
            case NUMBER:
            {
                if(tok.value().find('.') != std::string::npos)
                    return object_ptr_t(new number<float>(tok.value()));
                else
                    return object_ptr_t(new number<int>(tok.value()));
            }
            case QUOTE:
                tok.next_token();
                return object_ptr_t(new quote(compile_expr(env, tok)));
            default:
                assert(false);
                return nil();
            }
        }

        object_ptr_t compile_string(environment* env, const std::string& str)
        {
            return nil();
        }
    }
}

#endif  // LISP_INTERPRETER_HPP
