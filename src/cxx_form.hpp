#ifndef CXX_FORM_HPP
#define CXX_FORM_HPP

#include "object.hpp"


namespace lisp {
    class cxx_function_callback : public object
    {
    public:	
        typedef std::vector<object_ptr_t> argv_t;
	typedef object_ptr_t (*callback_t)(environment*, const argv_t&);

	cxx_function_callback(callback_t cb);

	object_ptr_t operator()(environment* env,
				const cons_cell_ptr_t args);

    private:
	callback_t m_cb;
    };
}

#endif  // CXX_FORM_HPP
