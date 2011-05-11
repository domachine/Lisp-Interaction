// #define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE main_test

#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include "lisp.hpp"
#include "interpreter.hpp"
#include "types.hpp"
#include "function.hpp"


BOOST_AUTO_TEST_CASE(test_gc)
{
    BOOST_TEST_MESSAGE("fetching symbol from global_env ...");
    lisp::object_ptr_t sym = lisp::global_env()->get_symbol("test-symbol");

    BOOST_TEST_MESSAGE("constructing local environment and testing garbage collection ...");

    lisp::symbol_ptr_t symbol;

    {
        lisp::environment env(lisp::global_env());

        symbol = env.get_symbol("throw-away-symbol");
    }

    BOOST_TEST_MESSAGE("garbage collection deadline, returning");
    // return 0;
}

BOOST_AUTO_TEST_CASE(test_print)
{
    lisp::object_ptr_t sym = lisp::global_env()->get_symbol("test-sym");

    BOOST_TEST_MESSAGE(sym->str());

    // return 0;
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
            BOOST_TEST_MESSAGE("** Hello world from C in lisp **");

            lisp::object_ptr_t cdr = args->cdr();

            while(cdr && *cdr) {
                if(!cdr->is_cons_cell()) {
                    BOOST_TEST_MESSAGE("arg: " + cdr->str());
                    cdr = lisp::nil();
                }
                else {
                    BOOST_TEST_MESSAGE("arg: "
				       + env->eval(
					   boost::dynamic_pointer_cast<lisp::cons_cell>(cdr)->car())->str());
                    cdr = boost::dynamic_pointer_cast<lisp::cons_cell>(cdr)->cdr();
                }
            }

            return lisp::nil();
        }
};

BOOST_AUTO_TEST_CASE(test_callable)
{
    lisp::symbol_ptr_t sym = lisp::global_env()->get_symbol("function-symbol");
    sym->set_function(lisp::object_ptr_t(new function()));

    lisp::global_env()->eval(lisp::object_ptr_t(new lisp::cons_cell(sym)));
}

BOOST_AUTO_TEST_CASE(test_nil_t)
{
    BOOST_CHECK_EQUAL(lisp::nil(), lisp::nil());
    BOOST_CHECK_EQUAL(lisp::t(), lisp::t());
}

BOOST_AUTO_TEST_CASE(test_parser)
{
    using lisp::tokenizer;
    std::string script("   \"Hello\"");

    std::string::iterator iter = script.begin();
    tokenizer<std::string::iterator> tok(iter, script.end());

    script = "   (\"huhu \" asdasd 3+)";

    iter = script.begin();
    tokenizer<std::string::iterator> tok2(iter, script.end());

    while(tok2.next_token())
        BOOST_TEST_MESSAGE("token: " + tok2.value());
}

BOOST_AUTO_TEST_CASE(test_interpreter)
{
    lisp::global_env()->get_symbol("hello-world")->set_function(
        lisp::object_ptr_t(new function()));

    std::string script("(lambda (a) (if nil (hello-world 'a) (hello-world a)))");
    std::string::iterator iter = script.begin();

    lisp::tokenizer<std::string::iterator> tok(iter, script.end());
    tok.next_token();

    lisp::object_ptr_t obj = lisp::interpreter::compile_expr(lisp::global_env(), tok);

    BOOST_TEST_MESSAGE("function: " + lisp::global_env()->eval(obj)->str());

    lisp::object_ptr_t func = lisp::global_env()->eval(obj);

    lisp::global_env()->funcall(func,
                                lisp::cons_cell_ptr_t(
                                    new lisp::cons_cell(
                                        func,
                                        lisp::cons_cell_ptr_t(
                                            new lisp::cons_cell(
                                                lisp::object_ptr_t(
                                                    new lisp::number(4.5)))))));
}

BOOST_AUTO_TEST_CASE(number_test)
{
    using lisp::number;
    using lisp::number_ptr_t;

    lisp::number_ptr_t num = lisp::number_ptr_t(new number(1, 2));

    BOOST_CHECK_EQUAL((*num) + number(static_cast<double>(0.5)),
		      number(static_cast<long long>(1)));

    BOOST_CHECK_EQUAL(*num, number(0.5));
    BOOST_CHECK_EQUAL(*num, number(1, 2));
}

/*
  Usage: ./lisp-test <file>

  File will be interpreted as lisp script.
*/
// int main(int argc, char **argv)
// {
//     logging::init(std::cerr);

//     if(argc == 1)
//         return test_gc() +
//             test_print() +
//             test_callable() +
//             test_nil_t() +
//             test_parser() +
//             test_interpreter();
//     else {
//         using lisp::tokenizer;
//         using lisp::object_ptr_t;

//         const char* file = argv[1];

//         std::ifstream infile(file);
//         infile.unsetf(std::ios::skipws);

//         std::istream_iterator<char> iter(infile);
//         lisp::tokenizer<std::istream_iterator<char> > tok(iter,
//                                                           std::istream_iterator<char>());

//         while(tok.next_token()) {
//             object_ptr_t c = lisp::interpreter::compile_expr(lisp::global_env(), tok);
//             lisp::global_env()->eval(c);
//         }
//     }
// }
