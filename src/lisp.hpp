
#ifndef LISP_HPP
#define LISP_HPP

#include <vector>
#include <map>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "types.hpp"


namespace lisp {
    /**
       @brief Represents the nil object. Always returns the
       pointer to same object. @c nil is unique.
    */
    const object_ptr_t nil();

    const object_ptr_t t();

    environment* global_env();


    /**
       @brief Special t-object that does nothing but
       returning `true' in boolean context.
    */
    class t_object : public object
    {
    public:
        operator bool() const
            {
                return true;
            }

        std::string str() const
            {
                return "t";
            }

        friend const object_ptr_t t();

    private:
        t_object()
            : object()
            {
            }
    };

    class nil_object : public object
    {
    public:
        operator bool() const
            {
                return false;
            }

        std::string str() const
            {
                return "nil";
            }

        friend const object_ptr_t nil();

    private:
        // Make it impossible to instantiate an
        // object without using the nil() function.
        nil_object()
            : object()
            {
            }
    };

    /**
       @brief One of lisp's most important objects.

       Used for building lists for example.
    */
    class cons_cell : public object
    {
    public:
        cons_cell(object_ptr_t car = nil(),
                  object_ptr_t cdr = nil());

        object_ptr_t car() const;

	void set_car(object_ptr_t car);

        object_ptr_t cdr() const;

	void set_cdr(object_ptr_t cdr);

        bool empty() const;

        bool is_cons_cell() const;

        std::string str() const;

    protected:
        object_ptr_t eval(environment* env);

    private:
        object_ptr_t m_car;
        object_ptr_t m_cdr;
    };


    class symbol_ref;

    /**
       @brief The class that represents a lisp-symbol.

       This class cannot be instantiated manually.
       Because the garbage collector needs to be sure that
       a symbol only exists once.

       So use environment::get_symbol() to get a pointer
       to a symbol.
    */
    class symbol : public object
    {
    public:
        friend class symbol_ref;
        friend class environment;

        const std::string& name() const
            {
                return m_name;
            }

        object_ptr_t value() const;

        object_ptr_t function() const;

        object_ptr_t property_list() const
            {
                assert(m_property_list);
                return m_property_list;
            }

        void set_value(object_ptr_t obj)
            {
                m_value = obj;
            }

        void set_function(object_ptr_t obj)
            {
                m_function = obj;
            }

        /**
           @brief Returns the environment in which the object
           lives.
        */
        environment* env() const
            {
                return m_env;
            }

        void set_env(environment* env)
            {
                m_env = env;
            }

        bool is_symbol() const
            {
                return true;
            }

        /**
           @brief Indicates whether the symbol is useless
           which means that all cells are empty or @c nil.

           If this happens the symbol will be proposed to the
           garbage collector.
        */
        bool is_useless() const;

        std::string str() const
            {
                return m_name;
            }

    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t());

        object_ptr_t eval(environment*)
            {
                return value();
            }

    private:
        symbol()
            {
                // This should not happen.
                assert(false);
            }

        symbol(environment* env, const std::string& name)
            : object(),
              m_name(name),
              m_property_list(nil()),
              m_env(env)
            {
            }

        // Disable copying
        symbol(const symbol&)
            : object()
            {
                assert(false);
            }

        std::string m_name;
        object_ptr_t m_value;
        object_ptr_t m_function;
        object_ptr_t m_property_list;
        environment* m_env;
    };

    typedef boost::shared_ptr<symbol> symbol_ptr_t;

    class symbol_ref : public object
    {
    public:
        symbol_ref(const std::string& name)
            : m_name(name)
            {
            }

        std::string str() const
            {
                return m_name;
            }

        const std::string& name() const
            {
                return m_name;
            }

        bool is_symbol_ref() const
            {
                return true;
            }

    protected:
        object_ptr_t operator()(environment* env,
                                const cons_cell_ptr_t args = cons_cell_ptr_t());

        object_ptr_t eval(environment* env);

    private:
        std::string m_name;
    };

    typedef boost::shared_ptr<symbol_ref> symbol_ref_ptr_t;

    class quote : public object
    {
    public:
        /**
           @brief Constructs a new quote object.

           Simply holds the given object.
        */
        quote(object_ptr_t obj)
            : object(),
              m_object(obj)
            {
                assert(m_object);
            }

        std::string str() const
            {
                return "'" + m_object->str();
            }

    protected:
        /**
           @brief On evaluation simply return the
           object.

           @return A pointer to the hold object.
        */
        object_ptr_t eval(environment*)
            {
                return m_object;
            }

    private:
        object_ptr_t m_object;
    };

    void signal(symbol_ptr_t err_sym, const std::string& what);

    /**
       @brief Handles a symbol table and takes care
       that the symbols are destroyed if they aren't needed
       anymore.

       @see symbol::is_useless().
    */
    class environment
    {
        struct deleter
        {
            void operator()(symbol* sym);
        };

    public:
        typedef std::pair<symbol*, int> symbol_entry_t;
        typedef std::map<std::string, symbol_entry_t> symbol_table_t;

        environment(environment* parent = 0);
        ~environment();

        /**
           @brief Creates symbol `name' explicitely without checking
           parent.

           Should be used for creating let-like environments only.
           In other contexts it doesn't make sense. Use get_symbol()
           instead.

           @throws std::logic_error if the symbol already exists.
        */
        symbol_ptr_t create_symbol(const std::string& name);

        /**
           @brief Checks if the named symbol exists otherwise
           creates a new instance and returns a controlled pointer to
           it.

           @param name The name of the symbol to fetch.
        */
        symbol_ptr_t get_symbol(const std::string& name);

        /**
           @brief Evaluates the given object by calling the
           its eval() method and interpreting return-code.

           If the return-code is a null-pointer the given object is
           returned otherwise the returned pointer.

           @param obj The object to evaluate.
           @return See description.
        */
        object_ptr_t eval(object_ptr_t obj);

        /**
           @brief Used to call a function object.

           Calls the object's operator() method and interprets
           the return-code. If the method returns a null-pointer
           funcall() throws a lisp exception to signal that this
           is not a function object.

           @param obj The object to call.
           @param args The arguments to pass to the function object.
           @return The result of the called function.
         */
        object_ptr_t funcall(object_ptr_t obj,
                             const cons_cell_ptr_t args = cons_cell_ptr_t());

    private:
        void del_ref(const std::string& name);

        symbol_table_t m_symbols;

        environment* m_parent;
    };
}

#endif  // LISP_HPP
