#ifndef LISP_OBJECT_HPP
#define LISP_OBJECT_HPP

#include <vector>

#include <boost/shared_ptr.hpp>


namespace lisp {
    // Forward declaration.
    class object;
    typedef boost::shared_ptr<object> object_ptr_t;

    class environment;

    class cons_cell;
    typedef boost::shared_ptr<cons_cell> cons_cell_ptr_t;


    /**
       @brief Base class for all objects.
    */
    class object
    {
    public:
        typedef std::vector<object_ptr_t> arglist_t;

        virtual ~object()
            {
            }

        virtual bool is_cons_cell() const
            {
                return false;
            }

        /**
           @brief Is overridden by the symbol class and
           should only return true in that case. Otherwise
           it should always return false.
        */
        virtual bool is_symbol_ref() const
            {
                return false;
            }

        /**
           @brief Returns a string that represents the object.
        */
        virtual std::string str() const;

        virtual operator bool() const;

        friend class environment;

    protected:
        /**
           @brief Is called by the eval function.

           If this operator returns a null pointer, it
           means that an object of that type is evaluated to
           itself.
        */
        virtual object_ptr_t eval(environment* env);

        /**
           @brief Can be overridden by object-classes. Is called in
           funcall contexts.

           For this method the same rules as for the eval() method
           exist. If it returns a null pointer, a exception is thrown
           to signal that this object is not callable.
        */
        virtual object_ptr_t operator()(environment* env,
                                        const cons_cell_ptr_t = cons_cell_ptr_t());
    };
}

#endif  // LISP_OBJECT_HPP
