
#include "logging.hpp"

#include "lisp.hpp"
#include "interpreter.hpp"
#include "types.hpp"
#include "function.hpp"

using logging::DEBUG;
using logging::log;

int test_gc()
{
    log(DEBUG) << "*** starting garbage collector test ***" << std::endl;
    
    log(DEBUG) << "fetching symbol from global_env ..." << std::endl;
    lisp::object_ptr_t sym = lisp::global_env()->get_symbol("test-symbol");

    log(DEBUG) << "constructing local environment and testing garbage collection ..."
               << std::endl;

    lisp::symbol_ptr_t symbol;

    {
        lisp::environment env(lisp::global_env());

        symbol = env.get_symbol("throw-away-symbol");
        symbol->set_value(lisp::t());
    }

    log(DEBUG) << "garbage collection deadline, returning" << std::endl;

    return 0;
}

int test_print()
{
    log(DEBUG) << "*** starting printable test ***" << std::endl;

    lisp::object_ptr_t sym = lisp::global_env()->get_symbol("test-sym");

    log(DEBUG) << sym->str() << std::endl;

    return 0;
}

class function : public lisp::object
{
public:
    function()
        : lisp::object()
        {
        }

protected:
    lisp::object_ptr_t operator()(lisp::environment* env, const lisp::cons_cell_ptr_t args)
        {
            log(DEBUG) << "** Hello world from C in lisp **" << std::endl;

            lisp::object_ptr_t cdr = args;

            while(cdr && *cdr) {
                log(DEBUG) << "arg: "
                           << boost::dynamic_pointer_cast<lisp::cons_cell>(cdr)->car()->str()
                           << std::endl;
                cdr = boost::dynamic_pointer_cast<lisp::cons_cell>(cdr)->cdr();
            }

            return lisp::nil();
        }
};

int test_callable()
{
    log(DEBUG) << "*** starting callable test ***" << std::endl;

    lisp::symbol_ptr_t sym = lisp::global_env()->get_symbol("function-symbol");
    sym->set_function(lisp::object_ptr_t(new function()));

    lisp::global_env()->eval(lisp::object_ptr_t(new lisp::cons_cell(sym)));
    log(DEBUG) << "evaluated: " << lisp::global_env()->eval(sym)->str() << std::endl;

    return 0;
}

int test_nil_t()
{
    log(DEBUG) << "*** starting nil and t equality test ***" << std::endl;

    log(DEBUG) << (lisp::nil() == lisp::nil()) << std::endl;

    log(DEBUG) << (lisp::t() == lisp::t()) << std::endl;

    return 0;
}

int test_parser()
{
    using lisp::tokenizer;
    std::string script("   \"Hello\"");

    std::string::iterator iter = script.begin();
    tokenizer<std::string::iterator> tok(iter, script.end());

    log(DEBUG) << tok.next_token() << ": " << tok.value() << std::endl;

    script = "   (\"huhu \" asdasd 3+)";

    iter = script.begin();
    tokenizer<std::string::iterator> tok2(iter, script.end());

    while(tok2.next_token()) {
        log(DEBUG) << "token: " << tok2.value() << std::endl;
    }

    return 0;
}

int test_interpreter()
{
    // lisp::global_env()->get_symbol("defun")->set_function(
    //     lisp::object_ptr_t(new lisp::defun_form()));
    lisp::global_env()->get_symbol("inline")->set_function(
        lisp::object_ptr_t(new function()));

    std::string script("(defun () (inline  3..1  list  sym))");
    std::string::iterator iter = script.begin();

    lisp::tokenizer<std::string::iterator> tok(iter, script.end());
    tok.next_token();

    lisp::object_ptr_t obj = lisp::interpreter::compile_expr(lisp::global_env(), tok);

    log(DEBUG) << "compiled list: " << obj->str() << std::endl;
    lisp::object_ptr_t func = lisp::global_env()->eval(obj);

    lisp::global_env()->funcall(func);

    return 0;
}


int main()
{
    logging::init(std::cerr);

    return test_gc() +
        test_print() +
        test_callable() +
        test_nil_t() +
        test_parser() +
        test_interpreter();
}
