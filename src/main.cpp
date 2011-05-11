// #define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE main_test

#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>

#include "lisp.hpp"
#include "interpreter.hpp"


/*
  Usage: ./lisp-test <file>

  File will be interpreted as lisp script.
*/
int main(int argc, char **argv)
{
    if(argc == 1) {
	std::cerr << "Usage: " << argv[0] << " <lisp-file>" << std::endl;
	return 1;
    }
    else {
        using lisp::tokenizer;
        using lisp::object_ptr_t;

        const char* file = argv[1];

        std::ifstream infile(file);
        infile.unsetf(std::ios::skipws);

        std::istream_iterator<char> iter(infile);
        lisp::tokenizer<std::istream_iterator<char> > tok(iter,
                                                          std::istream_iterator<char>());

        while(tok.next_token()) {
            object_ptr_t c = lisp::interpreter::compile_expr(lisp::global_env(), tok);
            lisp::global_env()->eval(c);
        }

	return 0;
    }
}
